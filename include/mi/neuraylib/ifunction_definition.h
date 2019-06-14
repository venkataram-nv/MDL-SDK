/***************************************************************************************************
 * Copyright (c) 2015-2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **************************************************************************************************/
/// \file
/// \brief      Scene element Function_definition

#ifndef MI_NEURAYLIB_IFUNCTION_DEFINITION_H
#define MI_NEURAYLIB_IFUNCTION_DEFINITION_H

#include <cstring>

#include <mi/neuraylib/iexpression.h>
#include <mi/neuraylib/iscene_element.h>

namespace mi {

namespace neuraylib {

/** \addtogroup mi_neuray_mdl_elements
@{
*/

class IFunction_call;

/// This interface represents a function definition.
///
/// A function definition describes the formal structure of a function call, i.e. the number,
/// types, names, and defaults of its parameters, as well as its return type. The
/// #create_function_call() method allows to create function calls based on this function
/// definition.
///
/// \see #mi::neuraylib::IFunction_call, #mi::neuraylib::IModule,
///      #mi::neuraylib::Definition_wrapper
class IFunction_definition : public
    mi::base::Interface_declare<0x3504744d,0xd45b,0x4a99,0xb6,0x21,0x10,0x9e,0xd5,0xcb,0x36,0xc1,
                                neuraylib::IScene_element>
{
public:

    /// All known semantics of functions definitions.
    ///
    /// \note Do not rely on the numeric values of the enumerators since they may change without
    ///       further notice.
    enum Semantics
    {
        DS_UNKNOWN = 0,                            ///< Unknown semantics.

        DS_CONV_CONSTRUCTOR,                      ///< The conversion constructor.
        DS_ELEM_CONSTRUCTOR,                      ///< The elemental constructor.
        DS_COLOR_SPECTRUM_CONSTRUCTOR,            ///< The color from spectrum constructor.
        DS_MATRIX_ELEM_CONSTRUCTOR,               ///< The matrix elemental constructor.
        DS_MATRIX_DIAG_CONSTRUCTOR,               ///< The matrix diagonal constructor.
        DS_INVALID_REF_CONSTRUCTOR,               ///< The invalid reference constructor.
        DS_DEFAULT_STRUCT_CONSTRUCTOR,            ///< The default constructor for a struct.
        DS_TEXTURE_CONSTRUCTOR,                   ///< The texture constructor.

        DS_CONV_OPERATOR,                         ///< The type conversion operator.

        // Unary operators
        DS_OPERATOR_FIRST = 0x0200,
        DS_UNARY_FIRST = DS_OPERATOR_FIRST,
        DS_BITWISE_COMPLEMENT = DS_UNARY_FIRST,   ///< The bitwise complement operator.
        DS_LOGICAL_NOT,                           ///< The unary logical negation operator.
        DS_POSITIVE,                              ///< The unary arithmetic positive operator.
        DS_NEGATIVE,                              ///< The unary arithmetic negation operator.
        DS_PRE_INCREMENT,                         ///< The pre-increment operator.
        DS_PRE_DECREMENT,                         ///< The pre-decrement operator.
        DS_POST_INCREMENT,                        ///< The post-increment operator.
        DS_POST_DECREMENT,                        ///< The post-decrement operator.
        DS_CAST,                                  ///< The cast operator.
        DS_UNARY_LAST = DS_CAST,

        // Binary operators
        DS_BINARY_FIRST,
        DS_SELECT = DS_BINARY_FIRST,              ///< The select operator.
        DS_ARRAY_INDEX,                           ///< The array index operator.
        DS_MULTIPLY,                              ///< The multiplication operator.
        DS_DIVIDE,                                ///< The division operator.
        DS_MODULO,                                ///< The modulus operator.
        DS_PLUS,                                  ///< The addition operator.
        DS_MINUS,                                 ///< The subtraction operator.
        DS_SHIFT_LEFT,                            ///< The shift-left operator.
        DS_SHIFT_RIGHT,                           ///< The arithmetic shift-right operator.
        DS_UNSIGNED_SHIFT_RIGHT,                  ///< The unsigned shift-right operator.
        DS_LESS,                                  ///< The less operator.
        DS_LESS_OR_EQUAL,                         ///< The less-or-equal operator.
        DS_GREATER_OR_EQUAL,                      ///< The greater-or-equal operator.
        DS_GREATER,                               ///< The greater operator.
        DS_EQUAL,                                 ///< The equal operator.
        DS_NOT_EQUAL,                             ///< The not-equal operator.
        DS_BITWISE_AND,                           ///< The bitwise and operator.
        DS_BITWISE_XOR,                           ///< The bitwise xor operator.
        DS_BITWISE_OR,                            ///< The bitwise or operator.
        DS_LOGICAL_AND,                           ///< The logical and operator.
        DS_LOGICAL_OR,                            ///< The logical or operator.
        DS_ASSIGN,                                ///< The assign operator.
        DS_MULTIPLY_ASSIGN,                       ///< The multiplication-assign operator.
        DS_DIVIDE_ASSIGN,                         ///< The division-assign operator.
        DS_MODULO_ASSIGN,                         ///< The modulus-assign operator.
        DS_PLUS_ASSIGN,                           ///< The plus-assign operator.
        DS_MINUS_ASSIGN,                          ///< The minus-assign operator.
        DS_SHIFT_LEFT_ASSIGN,                     ///< The shift-left-assign operator.
        DS_SHIFT_RIGHT_ASSIGN,                    ///< The arithmetic shift-right-assign operator.
        DS_UNSIGNED_SHIFT_RIGHT_ASSIGN,           ///< The unsigned shift-right-assign operator.
        DS_BITWISE_OR_ASSIGN,                     ///< The bitwise or-assign operator.
        DS_BITWISE_XOR_ASSIGN,                    ///< The bitwise xor-assign operator.
        DS_BITWISE_AND_ASSIGN,                    ///< The bitwise and-assign operator.
        DS_SEQUENCE,                              ///< The comma operator.
        DS_BINARY_LAST = DS_SEQUENCE,

        // Ternary operator
        DS_TERNARY,                               ///< The ternary operator (conditional).
        DS_OPERATOR_LAST = DS_TERNARY,

        // ::math module intrinsics
        DS_INTRINSIC_MATH_FIRST = 0x0300,
        DS_INTRINSIC_MATH_ABS                     ///< The %math::abs() intrinsic function.
            = DS_INTRINSIC_MATH_FIRST,
        DS_INTRINSIC_MATH_ACOS,                   ///< The %math::acos() intrinsic function.
        DS_INTRINSIC_MATH_ALL,                    ///< The %math::all() intrinsic function.
        DS_INTRINSIC_MATH_ANY,                    ///< The %math::any() intrinsic function.
        DS_INTRINSIC_MATH_ASIN,                   ///< The %math::asin() intrinsic function.
        DS_INTRINSIC_MATH_ATAN,                   ///< The %math::atan() intrinsic function.
        DS_INTRINSIC_MATH_ATAN2,                  ///< The %math::atan2() intrinsic function.
        DS_INTRINSIC_MATH_AVERAGE,                ///< The %math::average() intrinsic function.
        DS_INTRINSIC_MATH_CEIL,                   ///< The %math::ceil() intrinsic function.
        DS_INTRINSIC_MATH_CLAMP,                  ///< The %math::clamp() intrinsic function.
        DS_INTRINSIC_MATH_COS,                    ///< The %math::cos() intrinsic function.
        DS_INTRINSIC_MATH_CROSS,                  ///< The %math::cross() intrinsic function.
        DS_INTRINSIC_MATH_DEGREES,                ///< The %math::degrees() intrinsic function.
        DS_INTRINSIC_MATH_DISTANCE,               ///< The %math::distance() intrinsic function.
        DS_INTRINSIC_MATH_DOT,                    ///< The %math::dot() intrinsic function.
        DS_INTRINSIC_MATH_EVAL_AT_WAVELENGTH,     ///< The %math::eval_at_wavelength()
                                                  ///  intrinsic function.
        DS_INTRINSIC_MATH_EXP,                    ///< The %math::exp() intrinsic function.
        DS_INTRINSIC_MATH_EXP2,                   ///< The %math::exp2() intrinsic function.
        DS_INTRINSIC_MATH_FLOOR,                  ///< The %math::floor() intrinsic function.
        DS_INTRINSIC_MATH_FMOD,                   ///< The %math::fmod() intrinsic function.
        DS_INTRINSIC_MATH_FRAC,                   ///< The %math::frac() intrinsic function.
        DS_INTRINSIC_MATH_ISNAN,                  ///< The %math::isnan() intrinsic function.
        DS_INTRINSIC_MATH_ISFINITE,               ///< The %math::isfinite() intrinsic function.
        DS_INTRINSIC_MATH_LENGTH,                 ///< The %math::length() intrinsic function.
        DS_INTRINSIC_MATH_LERP,                   ///< The %math::lerp() intrinsic function.
        DS_INTRINSIC_MATH_LOG,                    ///< The %math::log() intrinsic function.
        DS_INTRINSIC_MATH_LOG2,                   ///< The %math::log2() intrinsic function.
        DS_INTRINSIC_MATH_LOG10,                  ///< The %math::log10() intrinsic function.
        DS_INTRINSIC_MATH_LUMINANCE,              ///< The %math::luminance() intrinsic function.
        DS_INTRINSIC_MATH_MAX,                    ///< The %math::max() intrinsic function.
        DS_INTRINSIC_MATH_MAX_VALUE,              ///< The %math::max_value() intrinsic function.
        DS_INTRINSIC_MATH_MAX_VALUE_WAVELENGTH,   ///< The %math::max_value_wavelength()
                                                  ///  intrinsic function.
        DS_INTRINSIC_MATH_MIN,                    ///< The %math::min() intrinsic function.
        DS_INTRINSIC_MATH_MIN_VALUE,              ///< The %math::min_value() intrinsic function.
        DS_INTRINSIC_MATH_MIN_VALUE_WAVELENGTH,   ///< The %math::min_value_wavelength()
                                                  ///  intrinsic function.
        DS_INTRINSIC_MATH_MODF,                   ///< The %math::modf() intrinsic function.
        DS_INTRINSIC_MATH_NORMALIZE,              ///< The %math::normalize() intrinsic function.
        DS_INTRINSIC_MATH_POW,                    ///< The %math::pow() intrinsic function.
        DS_INTRINSIC_MATH_RADIANS,                ///< The %math::radians() intrinsic function.
        DS_INTRINSIC_MATH_ROUND,                  ///< The %math::round() intrinsic function.
        DS_INTRINSIC_MATH_RSQRT,                  ///< The %math::rsqrt() intrinsic function.
        DS_INTRINSIC_MATH_SATURATE,               ///< The %math::saturate() intrinsic function.
        DS_INTRINSIC_MATH_SIGN,                   ///< The %math::sign() intrinsic function.
        DS_INTRINSIC_MATH_SIN,                    ///< The %math::sin() intrinsic function.
        DS_INTRINSIC_MATH_SINCOS,                 ///< The %math::sincos() intrinsic function.
        DS_INTRINSIC_MATH_SMOOTHSTEP,             ///< The %math::smoothstep() intrinsic function.
        DS_INTRINSIC_MATH_SQRT,                   ///< The %math::sqrt() intrinsic function.
        DS_INTRINSIC_MATH_STEP,                   ///< The %math::step() intrinsic function.
        DS_INTRINSIC_MATH_TAN,                    ///< The %math::tan() intrinsic function.
        DS_INTRINSIC_MATH_TRANSPOSE,              ///< The %math::transpose() intrinsic function.
        DS_INTRINSIC_MATH_BLACKBODY,              ///< The %math::blackbody() intrinsic function.
        DS_INTRINSIC_MATH_EMISSION_COLOR,         ///< The %math::emission_color() intrinsic
                                                  ///  function.
        DS_INTRINSIC_MATH_DX,                     ///< The %math::DX() intrinsic function.
        DS_INTRINSIC_MATH_DY,                     ///< The %math::DY() intrinsic function.
        DS_INTRINSIC_MATH_LAST = DS_INTRINSIC_MATH_DY,

        // ::state module intrinsics
        DS_INTRINSIC_STATE_FIRST = 0x0400,
        DS_INTRINSIC_STATE_POSITION               ///< The %state::position() function.
            = DS_INTRINSIC_STATE_FIRST,
        DS_INTRINSIC_STATE_NORMAL,                ///< The %state::normal() function.
        DS_INTRINSIC_STATE_GEOMETRY_NORMAL,       ///< The %state::geometry_normal() function.
        DS_INTRINSIC_STATE_MOTION,                ///< The %state::motion() function.
        DS_INTRINSIC_STATE_TEXTURE_SPACE_MAX,     ///< The %state::texture_space_max() function.
        DS_INTRINSIC_STATE_TEXTURE_COORDINATE,    ///< The %state::texture_coordinate() function.
        DS_INTRINSIC_STATE_TEXTURE_TANGENT_U,     ///< The %state::texture_tangent_u() function.
        DS_INTRINSIC_STATE_TEXTURE_TANGENT_V,     ///< The %state::texture_tangent_v() function.
        DS_INTRINSIC_STATE_TANGENT_SPACE,         ///< The %state::tangent_space() function.
        DS_INTRINSIC_STATE_GEOMETRY_TANGENT_U,    ///< The %state::geometry_tangent_u() function.
        DS_INTRINSIC_STATE_GEOMETRY_TANGENT_V,    ///< The %state::geometry_tangent_v() function.
        DS_INTRINSIC_STATE_DIRECTION,             ///< The %state::direction() function.
        DS_INTRINSIC_STATE_ANIMATION_TIME,        ///< The %state::animation_time() function.
        DS_INTRINSIC_STATE_WAVELENGTH_BASE,       ///< The %state::wavelength_base() function.
        DS_INTRINSIC_STATE_TRANSFORM,             ///< The %state::transform() function.
        DS_INTRINSIC_STATE_TRANSFORM_POINT,       ///< The %state::transform_point() function.
        DS_INTRINSIC_STATE_TRANSFORM_VECTOR,      ///< The %state::transform_vector() function.
        DS_INTRINSIC_STATE_TRANSFORM_NORMAL,      ///< The %state::transform_normal() function.
        DS_INTRINSIC_STATE_TRANSFORM_SCALE,       ///< The %state::transform_scale() function.
        DS_INTRINSIC_STATE_ROUNDED_CORNER_NORMAL, ///< The %state::rounded_corner_normal() function.
        DS_INTRINSIC_STATE_METERS_PER_SCENE_UNIT, ///< The %state::meters_per_scene_unit() function.
        DS_INTRINSIC_STATE_SCENE_UNITS_PER_METER, ///< The %state::scene_units_per_meter() function.
        DS_INTRINSIC_STATE_OBJECT_ID,             ///< The %state::object_id() function.
        DS_INTRINSIC_STATE_WAVELENGTH_MIN,        ///< The %state::wavelength_min() function.
        DS_INTRINSIC_STATE_WAVELENGTH_MAX,        ///< The %state::wavelength_max() function.
        DS_INTRINSIC_STATE_LAST = DS_INTRINSIC_STATE_WAVELENGTH_MAX,

        // ::tex module intrinsics
        DS_INTRINSIC_TEX_FIRST = 0x0500,
        DS_INTRINSIC_TEX_WIDTH                    ///< The tex::width() function.
            = DS_INTRINSIC_TEX_FIRST,
        DS_INTRINSIC_TEX_HEIGHT,                  ///< The tex::height() function.
        DS_INTRINSIC_TEX_DEPTH,                   ///< The tex::depth() function.
        DS_INTRINSIC_TEX_LOOKUP_FLOAT,            ///< The tex::lookup_float() function.
        DS_INTRINSIC_TEX_LOOKUP_FLOAT2,           ///< The tex::lookup_float2() function.
        DS_INTRINSIC_TEX_LOOKUP_FLOAT3,           ///< The tex::lookup_float3() function.
        DS_INTRINSIC_TEX_LOOKUP_FLOAT4,           ///< The tex::lookup_float4() function.
        DS_INTRINSIC_TEX_LOOKUP_COLOR,            ///< The tex::lookup_color() function.
        DS_INTRINSIC_TEX_TEXEL_FLOAT,             ///< The tex::texel_float() function.
        DS_INTRINSIC_TEX_TEXEL_FLOAT2,            ///< The tex::texel_float2() function.
        DS_INTRINSIC_TEX_TEXEL_FLOAT3,            ///< The tex::texel_float3() function.
        DS_INTRINSIC_TEX_TEXEL_FLOAT4,            ///< The tex::texel_float4() function.
        DS_INTRINSIC_TEX_TEXEL_COLOR,             ///< The tex::texel_color() function.
        DS_INTRINSIC_TEX_TEXTURE_ISVALID,         ///< The tex::texture_isvalid() function.
        DS_INTRINSIC_TEX_LAST = DS_INTRINSIC_TEX_TEXTURE_ISVALID,

        // ::df module intrinsics
        DS_INTRINSIC_DF_FIRST = 0x0600,

        DS_INTRINSIC_DF_DIFFUSE_REFLECTION_BSDF   ///< The df::diffuse_reflection_bsdf() function.
            = DS_INTRINSIC_DF_FIRST,
        DS_INTRINSIC_DF_DIFFUSE_TRANSMISSION_BSDF,///< The df::diffuse_transmission_bsdf() function.
        DS_INTRINSIC_DF_SPECULAR_BSDF,            ///< The df::specular_bsdf() function.
        DS_INTRINSIC_DF_SIMPLE_GLOSSY_BSDF,       ///< The df::simple_glossy_bsdf() function.

        /// The df::backscattering_glossy_reflection_bsdf() function.
        DS_INTRINSIC_DF_BACKSCATTERING_GLOSSY_REFLECTION_BSDF,
        DS_INTRINSIC_DF_MEASURED_BSDF,            ///< The df::measured_bsdf() function.
        DS_INTRINSIC_DF_DIFFUSE_EDF,              ///< The df::diffuse_edf() function.
        DS_INTRINSIC_DF_MEASURED_EDF,             ///< The df::measured_edf() function.
        DS_INTRINSIC_DF_SPOT_EDF,                 ///< The df::spot_edf() function.
        DS_INTRINSIC_DF_ANISOTROPIC_VDF,          ///< The df::anisotropic_vdf() function.
        DS_INTRINSIC_DF_NORMALIZED_MIX,           ///< The df::normalized_mix() function.
        DS_INTRINSIC_DF_CLAMPED_MIX,              ///< The df::clamped_mix() function.
        DS_INTRINSIC_DF_WEIGHTED_LAYER,           ///< The df::weighted_layer() function.
        DS_INTRINSIC_DF_FRESNEL_LAYER,            ///< The df::fresnel_layer() function.
        DS_INTRINSIC_DF_CUSTOM_CURVE_LAYER,       ///< The df::custom_curve_layer() function.
        DS_INTRINSIC_DF_MEASURED_CURVE_LAYER,     ///< The df::measured_curve_layer() function.
        DS_INTRINSIC_DF_THIN_FILM,                ///< The df::thin_film() function.
        DS_INTRINSIC_DF_TINT,                     ///< The df::tint() function.
        DS_INTRINSIC_DF_DIRECTIONAL_FACTOR,       ///< The df::directional_factor() function.
        DS_INTRINSIC_DF_MEASURED_CURVE_FACTOR,    ///< The df::measured_curve_factor() function.
        DS_INTRINSIC_DF_LIGHT_PROFILE_POWER,      ///< The df::light_profile_power() function.
        DS_INTRINSIC_DF_LIGHT_PROFILE_MAXIMUM,    ///< The df::light_profile_maximum() function.
        DS_INTRINSIC_DF_LIGHT_PROFILE_ISVALID,    ///< The df::light_profile_isvalid() function.
        DS_INTRINSIC_DF_BSDF_MEASUREMENT_ISVALID, ///< The df::bsdf_measurement_is_valid() function.
        /// The df::microfacet_beckmann_smith_bsdf() function.
        DS_INTRINSIC_DF_MICROFACET_BECKMANN_SMITH_BSDF,
        /// The df::microfacet_ggx_smith_bsdf() function.
        DS_INTRINSIC_DF_MICROFACET_GGX_SMITH_BSDF,
        /// The df::microfacet_beckmann_vcavities() function.
        DS_INTRINSIC_DF_MICROFACET_BECKMANN_VCAVITIES_BSDF,
        /// The df::microfacet_ggx_vcavities() function.
        DS_INTRINSIC_DF_MICROFACET_GGX_VCAVITIES_BSDF,
        /// The df::ward_geisler_moroder_bsdf() function.
        DS_INTRINSIC_DF_WARD_GEISLER_MORODER_BSDF,
        DS_INTRINSIC_DF_COLOR_NORMALIZED_MIX,     ///< The df::color_normalized_mix() function.
        DS_INTRINSIC_DF_COLOR_CLAMPED_MIX,        ///< The df::color_clamped_mix() function.
        DS_INTRINSIC_DF_COLOR_WEIGHTED_LAYER,     ///< The df::color_weigthed_layer() function.
        DS_INTRINSIC_DF_COLOR_FRESNEL_LAYER,      ///< The df::color_fresnel_layer() function.
        DS_INTRINSIC_DF_COLOR_CUSTOM_CURVE_LAYER, ///< The df::color_custom_curve_layer() function.
        /// The df::color_measured_curve_layer() function.
        DS_INTRINSIC_DF_COLOR_MEASURED_CURVE_LAYER,
        DS_INTRINSIC_DF_FRESNEL_FACTOR,           ///< The df::fresnel_factor() function.
        DS_INTRINSIC_DF_MEASURED_FACTOR,          ///< The df::measured_factor() function.
        DS_INTRINSIC_DF_CHIANG_HAIR_BSDF,         ///< The df::chiang_hair_bsdf() function.
        DS_INTRINSIC_DF_LAST = DS_INTRINSIC_DF_CHIANG_HAIR_BSDF,


        // ::debug module intrinsics
        DS_INTRINSIC_DEBUG_FIRST = 0x0800,

        DS_INTRINSIC_DEBUG_BREAKPOINT             ///< The debug::breakpoint() function.
            = DS_INTRINSIC_DEBUG_FIRST,
        DS_INTRINSIC_DEBUG_ASSERT,                ///< The debug::assert() function.
        DS_INTRINSIC_DEBUG_PRINT,                 ///< The debug::print() function.
        DS_INTRINSIC_DEBUG_LAST = DS_INTRINSIC_DEBUG_PRINT,

        // DAG backend intrinsics
        DS_INTRINSIC_DAG_FIRST = 0x0900,
        /// The structure field access function.
        DS_INTRINSIC_DAG_FIELD_ACCESS = DS_INTRINSIC_DAG_FIRST,
        DS_INTRINSIC_DAG_ARRAY_CONSTRUCTOR,       ///< The array constructor.
        DS_INTRINSIC_DAG_INDEX_ACCESS,            ///< The specific operator[].
        DS_INTRINSIC_DAG_ARRAY_LENGTH,            ///< The array length operator.
        DS_INTRINSIC_DAG_LAST = DS_INTRINSIC_DAG_ARRAY_LENGTH,

        DS_FORCE_32_BIT = 0xffffffffU             //   Undocumented, for alignment only.
    };

    /// Returns the DB name of the module containing this function definition.
    ///
    /// The type of the module is #mi::neuraylib::IModule.
    virtual const char* get_module() const = 0;

    /// Returns the MDL name of the function definition.
    ///
    /// \note The MDL name of the function definition is different from the name of the DB element.
    ///       Use #mi::neuraylib::ITransaction::name_of() to obtain the name of the DB element.
    ///
    /// \return         The MDL name of the function definition.
    virtual const char* get_mdl_name() const = 0;

    /// Returns the DB name of the prototype, or \c NULL if this function definition is not a
    /// variant.
    virtual const char* get_prototype() const = 0;

    /// Returns the semantic of this function definition.
    virtual Semantics get_semantic() const = 0;

    /// Indicates whether this definition represents the array constructor.
    ///
    /// \see \ref mi_neuray_mdl_arrays
    inline bool is_array_constructor() const { return strcmp( get_mdl_name(), "T[](...)") == 0; }

    /// Indicates whether the function definition is exported by its module.
    virtual bool is_exported() const = 0;

    /// Indicates whether the function definition is uniform.
    ///
    /// \note This includes, in addition to functions definitions that are explicitly marked as
    ///       uniform, also function definitions that are not explicitly marked either uniform or
    ///       varying and that have been analyzed by the MDL compiler to be uniform.
    virtual bool is_uniform() const = 0;

    /// Returns the return type.
    ///
    /// \return         The return type.
    virtual const IType* get_return_type() const = 0;

    /// Returns the return type.
    ///
    /// This templated member function is a wrapper of the non-template variant for the user's
    /// convenience. It eliminates the need to call
    /// #mi::base::IInterface::get_interface(const Uuid &)
    /// on the returned pointer, since the return type already is a pointer to the type \p T
    /// specified as template parameter.
    ///
    /// \tparam T       The interface type of the requested element
    /// \return         The return type.
    template<class T>
    const T* get_return_type() const
    {
        const IType* ptr_itype = get_return_type();
        if ( !ptr_itype)
            return 0;
        const T* ptr_T = static_cast<const T*>( ptr_itype->get_interface( typename T::IID()));
        ptr_itype->release();
        return ptr_T;
    }

    /// Returns the number of parameters.
    virtual Size get_parameter_count() const = 0;

    /// Returns the name of the parameter at \p index.
    ///
    /// \param index    The index of the parameter.
    /// \return         The name of the parameter, or \c NULL if \p index is out of range.
    virtual const char* get_parameter_name( Size index) const = 0;

    /// Returns the index position of a parameter.
    ///
    /// \param name     The name of the parameter.
    /// \return         The index of the parameter, or -1 if \p name is invalid.
    virtual Size get_parameter_index( const char* name) const = 0;

    /// Returns the types of all parameters.
    virtual const IType_list* get_parameter_types() const = 0;

    /// Returns the defaults of all parameters.
    ///
    /// \note Not all parameters have defaults. Hence, the indices in the returned expression list
    ///       do not necessarily coincide with the parameter indices of this definition. Therefore,
    ///       defaults should be retrieved via the name of the parameter instead of its index.
    virtual const IExpression_list* get_defaults() const = 0;

    /// Returns the enable_if conditions of all parameters.
    ///
    /// \note Not all parameters have a condition. Hence, the indices in the returned expression
    ///       list do not necessarily coincide with the parameter indices of this definition.
    ///       Therefore, conditions should be retrieved via the name of the parameter instead of
    ///       its index.
    virtual const IExpression_list* get_enable_if_conditions() const = 0;

    /// Returns the number of other parameters whose enable_if condition might depend on the
    /// argument of the given parameter.
    ///
    /// \param index    The index of the parameter.
    /// \return         The number of other parameters whose enable_if condition depends on this
    ///                 parameter argument.
    virtual Size get_enable_if_users(Size index) const = 0;

    /// Returns the index of a parameter whose enable_if condition might depend on the
    /// argument of the given parameter.
    ///
    /// \param index    The index of the parameter.
    /// \param u_index  The index of the enable_if user.
    /// \return         The index of a parameter whose enable_if condition depends on this
    ///                 parameter argument, or ~0 if indexes are out of range.
    virtual Size get_enable_if_user(Size index, Size u_index) const = 0;

    /// Returns the annotations of the function definition itself, or \c NULL if there are no such
    /// annotations.
    virtual const IAnnotation_block* get_annotations() const = 0;

    /// Returns the annotations of the return type of this function definition, or \c NULL if there
    /// are no such annotations.
    virtual const IAnnotation_block* get_return_annotations() const = 0;

    /// Returns the annotations of all parameters.
    ///
    /// \note Not all parameters have annotations. Hence, the indices in the returned annotation
    ///       list do not necessarily coincide with the parameter indices of this definition.
    ///       Therefore, annotation blocks should be retrieved via the name of the parameter
    ///       instead of its index.
    virtual const IAnnotation_list* get_parameter_annotations() const = 0;

    /// Returns the resolved file name of the thumbnail image for this function definition.
    ///
    /// The function first checks for a thumbnail annotation. If the annotation is provided,
    /// it uses the 'name' argument of the annotation and resolves that in the MDL search path.
    /// If the annotation is not provided or file resolution fails, it checks for a file
    /// module_name.material_name.png next to the MDL module.
    /// In case this cannot be found either \c NULL is returned.
    ///
    virtual const char* get_thumbnail() const = 0;

    /// Creates a new function call.
    ///
    /// \param arguments    The arguments of the created function call. \n
    ///                     Arguments for parameters without default are mandatory, otherwise
    ///                     optional. The type of an argument must match the corresponding parameter
    ///                     type. Any argument missing in \p arguments will be set to the default of
    ///                     the corresponding parameter. \n
    ///                     Note that the expressions in \p arguments are copied. This copy
    ///                     operation is a deep copy, e.g., DB elements referenced in call
    ///                     expressions are also copied. \n
    ///                     \c NULL is a valid argument which is handled like an empty expression
    ///                     list.
    /// \param[out] errors  An optional pointer to an #mi::Sint32 to which an error code will be
    ///                     written. The error codes have the following meaning:
    ///                     -  0: Success.
    ///                     - -1: An argument for a non-existing parameter was provided in
    ///                           \p arguments.
    ///                     - -2: The type of an argument in \p arguments does not have the correct
    ///                           type, see #get_parameter_types().
    ///                     - -3: A parameter that has no default was not provided with an argument
    ///                           value.
    ///                     - -4: The definition can not be instantiated because it is not exported.
    ///                     - -5: A parameter type is uniform, but the corresponding argument has a
    ///                           varying return type.
    ///                     - -6: An argument expression is not a constant nor a call.
    ///                     - -8: One of the parameter types is uniform, but the corresponding
    ///                           argument or default is a call expression and the return type of
    ///                           the called function definition is effectively varying since the
    ///                           function definition itself is varying.
    /// \return             The created function call, or \c NULL in case of errors.
    virtual IFunction_call* create_function_call(
        const IExpression_list* arguments, Sint32* errors = 0) const = 0;
};

mi_static_assert(sizeof(IFunction_definition::Semantics) == sizeof(Uint32));

/*@}*/ // end group mi_neuray_mdl_elements

} // namespace neuraylib

} // namespace mi

#endif // MI_NEURAYLIB_IFUNCTION_DEFINITION_H

