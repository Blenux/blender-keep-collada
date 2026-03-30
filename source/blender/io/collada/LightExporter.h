/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup collada
 */

#pragma once

#include "COLLADASWLibraryLights.h"
#include "COLLADASWStreamWriter.h"

#include "DNA_light_types.h"
#include "DNA_object_types.h"
#include "DNA_scene_types.h"

#include "ExportSettings.h"

class LightsExporter : COLLADASW::LibraryLights {
 public:
  LightsExporter(COLLADASW::StreamWriter *sw, BCExportSettings &export_settings);
  void exportLights(blender::Scene *sce);
  void operator()(blender::Object *ob);

 private:
  template<class Functor>
  void forEachLightObjectInExportSet(blender::Scene *sce, Functor &f, blender::LinkNode *export_set)
  {
    blender::LinkNode *node;
    for (node = export_set; node; node = node->next) {
      blender::Object *ob = (blender::Object *)node->link;

      if (ob->type == blender::OB_LAMP && ob->data) {
        f(ob);
      }
    }
  }
  
  bool exportBlenderProfile(COLLADASW::Light &cla, blender::Light *la);
  BCExportSettings &export_settings;
};
