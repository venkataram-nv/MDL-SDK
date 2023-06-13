/*-------------------------------------------------------------------------
ParserGen -- Generation of the Recursive Descent Parser
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
ported to C++ by Csaba Balazs, University of Szeged
extended by M. Loeberbauer & A. Woess, Univ. of Linz
with improvements by Pat Terry, Rhodes University

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

As an exception, it is allowed to write an extension of Coco/R that is
used as a plugin in non-free software.

If not otherwise stated, any source code generated by Coco/R (other than
Coco/R itself) does not fall under the GNU General Public License.
-------------------------------------------------------------------------*/

#include <ctype.h>
#include "ArrayList.h"
#include "ParserGen.h"
#include "Parser.h"
#include "BitArray.h"
#include "Scanner.h"
#include "Generator.h"

namespace Coco {

void ParserGen::Indent (int n) {
	for (int i = 1; i <= n; i++) fprintf(gen, "\t");
}

// use a switch if more than 5 alternatives and none starts with a resolver, and no LL1 warning
bool ParserGen::UseSwitch (Node *p) {
	BitArray *s1, *s2;
	if (p->typ != Node::alt)
		return false;
	int nAlts = 0;
	s1 = new BitArray(tab->terminals.Count);
	while (p != NULL) {
		s2 = tab->Expected0(p->sub, curSy);
		// must not optimize with switch statement, if there are ll1 warnings
		if (s1->Overlaps(s2)) { return false; }
		s1->Or(s2);
		++nAlts;
		// must not optimize with switch-statement, if alt uses a resolver expression
		if (p->sub->typ == Node::rslv) return false;
		p = p->down;
	}
	return nAlts > 5;
}
    
int ParserGen::GenNamespaceOpen(const char *nsName) {
	if (nsName == NULL || coco_string_length(nsName) == 0) {
		return 0;
	}
	const int len = coco_string_length(nsName);
	int startPos = 0;
	int nrOfNs = 0;
	do {
		int curLen = coco_string_indexof(nsName + startPos, COCO_CPP_NAMESPACE_SEPARATOR);
		if (curLen == -1) { curLen = len - startPos; }
		char *curNs = coco_string_create(nsName, startPos, curLen);
		fprintf(gen, "namespace %s {\n", curNs);
		coco_string_delete(curNs);
		startPos = startPos + curLen + 1;
		if (startPos < len && nsName[startPos] == COCO_CPP_NAMESPACE_SEPARATOR) {
			++startPos;
		}
		++nrOfNs;
	} while (startPos < len);
	return nrOfNs;
}

void ParserGen::GenNamespaceClose(int nrOfNs) {
	for (int i = 0; i < nrOfNs; ++i) {
		fprintf(gen, "} // namespace\n");
	}
}

// Escape backslashes.
static void write_escaped(FILE *gen, char const *s)
{
	for (char c = *s++; c != '\0'; c = *s++) {
		if (c == '\\')
			fprintf(gen, "\\\\");
		else
			fprintf(gen, "%c", c);
	}
}

void ParserGen::CopySourcePart (Position *pos, int indent) {
	// Copy text described by pos from atg to gen
	int ch, i;
	if (pos != NULL) {
		buffer->SetPos(pos->beg); ch = buffer->Read();
		if (tab->emitLines && pos->line) {
			fprintf(gen, "\n#line %d \"", pos->line);
			write_escaped(gen, tab->srcName);
			fprintf(gen, "\"\n");
		}
		Indent(indent);
		while (buffer->GetPos() <= pos->end) {
			while (ch == CR || ch == LF) {  // eol is either CR or CRLF or LF
				fprintf(gen, "\n"); Indent(indent);
				if (ch == CR) { ch = buffer->Read(); } // skip CR
				if (ch == LF) { ch = buffer->Read(); } // skip LF
				for (i = 1; i <= pos->col && (ch == ' ' || ch == '\t'); i++) {
					// skip blanks at beginning of line
					ch = buffer->Read();
				}
				if (buffer->GetPos() > pos->end) goto done;
			}
			fprintf(gen, "%c", ch);
			ch = buffer->Read();
		}
		done:
		if (indent > 0) fprintf(gen, "\n");
	}
}

void ParserGen::GenErrorMsg (ErrorType errTyp, Symbol *sym) {
	errorNr++;
	char format[1000];
	coco_snprintf(format, sizeof(format), "\t\t\tcase %d: s = \"", errorNr);
	coco_string_merge(err, format);
	switch (errTyp) {
	case tErr:
		if (sym->tokenKind == Symbol::litToken || sym->tokenKind == Symbol::fixedToken) {
                        char const *name = sym->name;
                        if (name[0] != '"') {
                            Iterator *iter = tab->literals->GetIterator();
                            while (iter->HasNext()) {
                                DictionaryEntry *e = iter->Next();
                                if (e->val == sym) {
                                    name = e->key;
                                    break;
                                }
                            }
                        }
			coco_snprintf(format, sizeof(format), "%s expected", tab->Escape(name));
			coco_string_merge(err, format);
		} else {
			coco_snprintf(format, sizeof(format), "%s expected", sym->name);
			coco_string_merge(err, format);
		}
		break;
	case altErr:
		coco_snprintf(format, sizeof(format), "invalid %s", sym->name);
		coco_string_merge(err, format);
		break;
	case syncErr:
		coco_snprintf(format, sizeof(format), "this symbol not expected in %s", sym->name);
		coco_string_merge(err, format);
		break;
	}
	coco_snprintf(format, sizeof(format), "\"; break;\n");
	coco_string_merge(err, format);
}

int ParserGen::NewCondSet (const BitArray *s) {
	 // skip symSet[0] (reserved for union of SYNC sets)
	for (int i = 1; i < symSet.Count; ++i) {
		if (Sets::Equals(s, (BitArray *)symSet[i])) {
			return i;
		}
	}
	return symSet.Add(s->Clone());
}

void ParserGen::GenCond (const BitArray *s, Node *p) {
	if (p->typ == Node::rslv) CopySourcePart(p->pos, 0);
	else {
		int n = Sets::Elements(s);
		if (n == 0)
			fprintf(gen, "false"); // happens if an ANY set matches no symbol
		else if (n <= maxTerm) {
			for (int i = 0; i < tab->terminals.Count; ++i) {
				Symbol *sym = (Symbol*)tab->terminals[i];
				if ((*s)[sym->n]) {
					fprintf(gen, "la->kind == ");
					WriteSymbolOrCode(gen, sym);
					--n;
					if (n > 0) fprintf(gen, " || ");
				}
			}
		} else
			fprintf(gen, "StartOf(%d)", NewCondSet(s));
	}
}

void ParserGen::PutCaseLabels (const BitArray *s) {
	for (int i = 0; i < tab->terminals.Count; ++i) {
		Symbol *sym = (Symbol*)tab->terminals[i];
		if ((*s)[sym->n]) {
			fprintf(gen, "case ");
			WriteSymbolOrCode(gen, sym);
			fprintf(gen, ": ");
		}
	}
}

void ParserGen::GenCode (Node *p, int indent, BitArray &isChecked) {
	Node *p2;
	BitArray *s1, *s2;
	while (p != NULL) {
		switch (p->typ) {
		case Node::nt:
			Indent(indent);
			fprintf(gen, "%s(", p->sym->name);
			CopySourcePart(p->pos, 0);
			fprintf(gen, ");\n");
			break;
		case Node::t:
			Indent(indent);
			// assert: if isChecked[p->sym->n] is true, then isChecked contains only p->sym->n
			if (isChecked[p->sym->n])
				fprintf(gen, "Get();\n");
			else {
				fprintf(gen, "Expect(");
				WriteSymbolOrCode(gen, p->sym);
				fprintf(gen, ");\n");
			}
			break;
		case Node::wt:
			Indent(indent);
			s1 = tab->Expected(p->next, curSy);
			s1->Or(tab->allSyncSets);
			fprintf(gen, "ExpectWeak(");
			WriteSymbolOrCode(gen, p->sym);
			fprintf(gen, ", %d);\n", NewCondSet(s1));
			break;
		case Node::any:
		{
			Indent(indent);
			int acc = Sets::Elements(p->set);
			if (tab->terminals.Count == (acc + 1) || (acc > 0 && Sets::Equals(p->set, &isChecked))) {
				// either this ANY accepts any terminal (the + 1 = end of file), or exactly what's allowed here
				fprintf(gen, "Get();\n");
			} else {
				GenErrorMsg(altErr, curSy);
				if (acc > 0) {
					fprintf(gen, "if ("); GenCond(p->set, p); fprintf(gen, ") Get(); else SynErr(%d);\n", errorNr);
				} else fprintf(gen, "SynErr(%d); // ANY node that matches no symbol\n", errorNr);
			}
			break;
		}
		case Node::eps:		// nothing
			break;
		case Node::rslv:	// nothing
			break;
		case Node::sem:
			CopySourcePart(p->pos, indent);
			break;
		case Node::sync:
			Indent(indent);
			GenErrorMsg(syncErr, curSy);
			fprintf(gen, "while (!("); GenCond(p->set, p); fprintf(gen, ")) {");
			fprintf(gen, "SynErr(%d); Get();", errorNr); fprintf(gen, "}\n");
			break;
		case Node::alt:
		{
			s1 = tab->First(p);
			bool equal = Sets::Equals(s1, &isChecked);
			bool useSwitch = UseSwitch(p);
			if (useSwitch) { Indent(indent); fprintf(gen, "switch (la->kind) {\n"); }
			p2 = p;
			while (p2 != NULL) {
				s1 = tab->Expected(p2->sub, curSy);
				Indent(indent);
				if (useSwitch) {
					PutCaseLabels(s1); fprintf(gen, "{\n");
				} else if (p2 == p) {
					fprintf(gen, "if ("); GenCond(s1, p2->sub); fprintf(gen, ") {\n");
				} else if (p2->down == NULL && equal) { fprintf(gen, "} else {\n");
				} else {
					fprintf(gen, "} else if (");  GenCond(s1, p2->sub); fprintf(gen, ") {\n");
				}
				GenCode(p2->sub, indent + 1, *s1);
				if (useSwitch) {
					Indent(indent + 1); fprintf(gen, "break;\n");
					Indent(indent); fprintf(gen, "}\n");
				}
				p2 = p2->down;
			}
			Indent(indent);
			if (equal) {
				fprintf(gen, "}\n");
			} else {
				GenErrorMsg(altErr, curSy);
				if (useSwitch) {
					fprintf(gen, "default: SynErr(%d); break;\n", errorNr);
					Indent(indent); fprintf(gen, "}\n");
				} else {
					fprintf(gen, "} else {\n");
					Indent(indent + 1); fprintf(gen, "SynErr(%d);\n", errorNr);
					Indent(indent); fprintf(gen, "}\n");
				}
			}
			break;
		}
		case Node::iter:
			Indent(indent);
			p2 = p->sub;
			fprintf(gen, "while (");
			if (p2->typ == Node::wt) {
				s1 = tab->Expected(p2->next, curSy);
				s2 = tab->Expected(p->next, curSy);
				fprintf(gen, "WeakSeparator(");
				WriteSymbolOrCode(gen, p2->sym);
				fprintf(gen, ",%d,%d) ", NewCondSet(s1), NewCondSet(s2));
				s1 = new BitArray(tab->terminals.Count);  // for inner structure
				if (p2->up || p2->next == NULL) p2 = NULL; else p2 = p2->next;
			} else {
				s1 = tab->First(p2);
				GenCond(s1, p2);
			}
			fprintf(gen, ") {\n");
			GenCode(p2, indent + 1, *s1);
			Indent(indent); fprintf(gen, "}\n");
			break;
		case Node::opt:
			s1 = tab->First(p->sub);
			Indent(indent);
			fprintf(gen, "if ("); GenCond(s1, p->sub); fprintf(gen, ") {\n");
			GenCode(p->sub, indent + 1, *s1);
			Indent(indent); fprintf(gen, "}\n");
			break;
		default:
			break;
		}
		if (p->typ != Node::eps && p->typ != Node::sem && p->typ != Node::sync)
			isChecked.SetAll(false);
		if (p->up)
			break;
		p = p->next;
	}
}


void ParserGen::GenTokensHeader() {
	Symbol *sym;
	bool isFirst = true;

	fprintf(gen, "\tenum TokenKind {\n");

	// tokens
	for (int i = 0; i < tab->terminals.Count; ++i) {
		sym = (Symbol*)tab->terminals[i];
		if (!isalpha(sym->name[0])) { continue; }

		if (isFirst) { isFirst = false; }
		else { fprintf(gen , ",\n"); }

		fprintf(gen , "\t\t%s%s=%d", tab->tokenPrefix, sym->name, sym->n);
	}
	// generate helper values
	if (!isFirst)
		fprintf(gen , ",\n");
	fprintf(gen, "\t\tmaxT=%d,\n", tab->terminals.Count - 1);
	fprintf(gen, "\t\tnoSym = %d", tab->noSym->n);

	// pragmas
	for (int i = 0; i < tab->pragmas.Count; ++i) {
		if (isFirst) { isFirst = false; }
		else { fprintf(gen , ",\n"); }

		sym = (Symbol*)tab->pragmas[i];
		fprintf(gen , "\t\t_%s=%d", sym->name, sym->n);
	}

	fprintf(gen, "\n\t};\n");
}

void ParserGen::GenCodePragmas() {
	for (int i = 0; i < tab->pragmas.Count; ++i) {
		Symbol *sym = (Symbol*)tab->pragmas[i];
		fprintf(gen, "\t\tif (la->kind == ");
		WriteSymbolOrCode(gen, sym);
		fprintf(gen, ") {\n");
		CopySourcePart(sym->semPos, 4);
		fprintf(gen, "\t\t}\n");
	}
}

void ParserGen::WriteSymbolOrCode(FILE *gen, const Symbol *sym) {
	if (!isalpha(sym->name[0])) {
		fprintf(gen, "%d /* %s */", sym->n, sym->name);
	} else {
		fprintf(gen, "%s%s", tab->tokenPrefix, sym->name);
	}
}

void ParserGen::GenProductionsHeader() {
	for (int i = 0; i < tab->nonterminals.Count; ++i) {
		Symbol *sym = (Symbol*)tab->nonterminals[i];
		curSy = sym;
		fprintf(gen, "\tvoid %s(", sym->name);
		CopySourcePart(sym->attrPos, 0);
		fprintf(gen, ");\n");
	}
}

void ParserGen::GenProductions() {
	for (int i = 0; i < tab->nonterminals.Count; ++i) {
		Symbol *sym = (Symbol*)tab->nonterminals[i];
		curSy = sym;
		fprintf(gen, "void Parser::%s(", sym->name);
		CopySourcePart(sym->attrPos, 0);
		fprintf(gen, ") {\n");
		CopySourcePart(sym->semPos, 2);
		BitArray isChecked(tab->terminals.Count);
		GenCode(sym->graph, 1, isChecked);
		fprintf(gen, "}\n\n");
	}
}

void ParserGen::InitSets() {
	fprintf(gen, "\tstatic bool const set[%d][%d] = {\n", symSet.Count, tab->terminals.Count+1);

	for (int i = 0; i < symSet.Count; ++i) {
		BitArray *s = (BitArray*)symSet[i];
		fprintf(gen, "\t\t{");
		int j = 0;
		Symbol *sym;
		for (int k = 0; k < tab->terminals.Count; ++k) {
			sym = (Symbol*)tab->terminals[k];
			if ((*s)[sym->n]) fprintf(gen, "T,"); else fprintf(gen, "x,");
			++j;
			if (j%4 == 0) fprintf(gen, " ");
		}
		if (i == symSet.Count - 1) {
			fprintf(gen, "x}\n");
		} else {
			fprintf(gen, "x},\n");
		}
	}
	fprintf(gen, "\t};\n\n");
}

void ParserGen::WriteParser () {
	Generator g = Generator(tab, errors);
	int oldPos = buffer->GetPos();  // Pos is modified by CopySourcePart
	symSet.Add(tab->allSyncSets);

	fram = g.OpenFrame("Parser.frame");
	gen  = g.OpenGen("Parser.h");

	for (int i = 0; i < tab->terminals.Count; ++i) {
		Symbol *sym = (Symbol*)tab->terminals[i];
		GenErrorMsg(tErr, sym);
	}

	g.GenCopyright();
	g.SkipFramePart("-->begin");

	g.CopyFramePart("-->prefix");
	g.GenPrefixFromNamespace();

	g.CopyFramePart("-->prefix");
	g.GenPrefixFromNamespace();

	g.CopyFramePart("-->headerdef");

	if (usingPos != NULL) {CopySourcePart(usingPos, 0); fprintf(gen, "\n");}
	g.CopyFramePart("-->namespace_open");
	int nrOfNs = GenNamespaceOpen(tab->nsName);

	g.CopyFramePart("-->constantsheader");
	GenTokensHeader();  /* ML 2002/09/07 write the token kinds */
	g.CopyFramePart("-->declarations"); CopySourcePart(tab->semDeclPos, 0);
	g.CopyFramePart("-->productionsheader"); GenProductionsHeader();
	g.CopyFramePart("-->namespace_close");
	GenNamespaceClose(nrOfNs);

	g.CopyFramePart("-->implementation");
	fclose(gen);

	// Source
	gen = g.OpenGen("Parser.cpp");

	g.GenCopyright();
	g.SkipFramePart("-->begin");
	g.CopyFramePart("-->namespace_open");
	nrOfNs = GenNamespaceOpen(tab->nsName);

	g.CopyFramePart("-->pragmas"); GenCodePragmas();
	g.CopyFramePart("-->productions"); GenProductions();
	g.CopyFramePart("-->parseRoot"); fprintf(gen, "\t%s();\n", tab->gramSy->name); if (tab->checkEOF) fprintf(gen, "\tExpect(0);");
	g.CopyFramePart("-->constants");
	g.CopyFramePart("-->initialization"); InitSets();
	g.CopyFramePart("-->errors"); fprintf(gen, "%s", err);
	g.CopyFramePart("-->namespace_close");
	GenNamespaceClose(nrOfNs);
	g.CopyFramePart(NULL);
	fclose(gen);
	buffer->SetPos(oldPos);
}


void ParserGen::WriteStatistics () {
	fprintf(trace, "\n");
	fprintf(trace, "%d terminals\n", tab->terminals.Count);
	fprintf(trace, "%d symbols\n", tab->terminals.Count + tab->pragmas.Count +
	                             tab->nonterminals.Count);
	fprintf(trace, "%d nodes\n", tab->nodes.Count);
	fprintf(trace, "%d sets\n", symSet.Count);
}


ParserGen::ParserGen (Parser *parser)
: maxTerm(3)
, CR('\r')
, LF('\n')
, usingPos(NULL)
, errorNr(-1)
, curSy(NULL)
, fram(NULL)
, gen(NULL)
, err(NULL)
, symSet()
, tab(parser->tab)
, trace(parser->trace)
, errors(parser->errors)
, buffer(parser->scanner->buffer)
{
}

}; // namespace
