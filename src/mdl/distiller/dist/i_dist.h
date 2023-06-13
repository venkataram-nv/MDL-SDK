//*****************************************************************************
// Copyright 2023 NVIDIA Corporation. All rights reserved.
//*****************************************************************************
/// \file
/// \brief MDL Distiller module and main function
///
//*****************************************************************************

#ifndef MDL_DISTILLER_DIST_I_DIST_H
#define MDL_DISTILLER_DIST_I_DIST_H

#include <mi/base/handle.h>
#include <mi/base/interface_declare.h>

#include <mi/mdl/mdl_generated_dag.h>
#include <mi/mdl/mdl_distiller_options.h>

#include <base/system/main/i_module.h>

namespace mi { namespace mdl { class IRule_matcher_event; } }

namespace MI {

namespace SYSTEM { class Module_registration_entry; }

namespace DIST {

/// Public interface of the DIST module.
class Dist_module : public SYSTEM::IModule
{
public:
    /// Returns the module registrations entry for the module.
    static SYSTEM::Module_registration_entry* get_instance();

    /// Returns the name of the module.
    static const char* get_name() { return "DIST"; }

    /// Returns the number of available distilling targets.
    virtual mi::Size get_target_count() const = 0;

    /// Returns the name of the distilling target at index position.
    virtual const char* get_target_name( mi::Size index) const = 0;

    /// Main function to distill an MDL material.
    ///
    /// Uses a DAG material instance as input, applies selected rule sets and returns the result
    /// as a new DAG material instance.
    /// The mdl module ::nvidia::distilling_support needs to be loaded before calling this 
    /// function. This can be done via MDL::load_distilling_support_module (see i_mdl_utilities.h)
    ///
    /// \param call_resolver     An MDL call resolver interface.
    /// \param event_handler     If non-NULL, an event handler interface used to report events
    ///                          during processing.
    /// \param material_instance The instance to "distill".
    /// \param target            Distilling target model
    /// \param error             An optional pointer to an #mi::Sint32 to which an error code will
    ///                          be written. The error codes have the following meaning:
    ///                          -  0: Success.
    ///                          - -1: Reserved for API layer.
    ///                          - -2: Reserved for API layer.
    ///                          - -3: Unspecified failure.
    /// \return                  The distilled material instance, or \c NULL in case of failure.
    virtual const mi::mdl::IGenerated_code_dag::IMaterial_instance* distill(
        mi::mdl::ICall_name_resolver& call_resolver,
        mi::mdl::IRule_matcher_event* event_handler,
        const mi::mdl::IGenerated_code_dag::IMaterial_instance* material_instance,
        const char* target,
        mi::mdl::Distiller_options* options,
        mi::Sint32* error) const = 0;

    /// Returns the number of required MDL modules for the given
    /// target.
    virtual mi::Size get_required_module_count(char const *target) const = 0;

    /// Returns the name of the required MDL module with the given
    /// index for the given target.
    virtual const char* get_required_module_name(char const *target, mi::Size index) const = 0;

    /// Returns the MDL source code of the required MDL module with
    /// the given index for the given target.
    virtual const char* get_required_module_code(char const *target, mi::Size index) const = 0;
};

} // namespace DIST
} // namespace MI

#endif // MDL_DISTILLER_DIST_I_DIST_H
