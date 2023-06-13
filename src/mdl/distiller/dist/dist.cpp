//*****************************************************************************
// Copyright 2023 NVIDIA Corporation. All rights reserved.
//*****************************************************************************
/// \file

#include "pch.h"

#include "i_dist.h"
#include "dist_module_impl.h"

#include <base/hal/time/i_time.h>
#include <base/lib/log/i_log_assert.h>
#include <base/lib/log/i_log_logger.h>
#include <mi/base/handle.h>
#include <mi/mdl/mdl_distiller_plugin.h>
#include <mi/mdl/mdl_distiller_plugin_api.h>

namespace MI {
namespace DIST {

const mi::mdl::IGenerated_code_dag::IMaterial_instance* Dist_module_impl::distill(
    mi::mdl::ICall_name_resolver& call_resolver,
    mi::mdl::IRule_matcher_event* event_handler,
    const mi::mdl::IGenerated_code_dag::IMaterial_instance* material_instance,
    const char* target,
    mi::mdl::Distiller_options* options,
    mi::Sint32* p_error) const
{
    TIME::Stopwatch stopwatch;
    stopwatch.start();

    mi::Sint32 dummy;
    mi::Sint32 &error = p_error != NULL ? *p_error : dummy;
    error = 0;
    ASSERT(M_DIST, material_instance);
    const mi::mdl::IGenerated_code_dag::IMaterial_instance* res = 0;
    // The "none" target is the only builtin target. It always exists.
    if( strcmp( "none", target) == 0) {
        res = material_instance;
        res->retain();
    } else {
        // not a builtin target, try registered targets from distiller plugins
        Target_to_index_map::const_iterator it = m_target_to_index_map.find( std::string( target));
        if ( it != m_target_to_index_map.end()) {
            mi::Size plugin_index = it->second.first;
            mi::Size target_index = it->second.second;
            mi::mdl::Mdl_distiller_plugin* distiller_plugin = m_plugins[plugin_index];

            mi::mdl::IDistiller_plugin_api* api = 
                mi::mdl::IDistiller_plugin_api::get_new_distiller_plugin_api(
                    material_instance, &call_resolver);
            res = distiller_plugin->distill( 
                *api, event_handler, material_instance, target_index, options, &error);
            api->release();
            if ( ! res)
                error = -3;
        } else {
            error = -2;
        }
    }
    
    stopwatch.stop();
    LOG::mod_log->info( M_DIST, LOG::Mod_log::C_COMPILER, "Finished 'Distilling' after %f seconds.",
                        stopwatch.elapsed());
    return res;
}


} // namespace DIST
} // namespace MI
