/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include <cstdlib>

#include "COLLADASWLibraryAnimationClips.h"
#include "DEG_depsgraph.hh"
#include "ExportSettings.h"

class AnimationClipExporter : COLLADASW::LibraryAnimationClips {
 private:
  blender::Depsgraph *depsgraph;
  blender::Scene *scene = nullptr;
  COLLADASW::StreamWriter *sw;
  BCExportSettings &export_settings;
  std::vector<std::vector<std::string>> anim_meta;

 public:
  AnimationClipExporter(blender::Depsgraph *depsgraph,
                        COLLADASW::StreamWriter *sw,
                        BCExportSettings &export_settings,
                        std::vector<std::vector<std::string>> anim_meta)
      : COLLADASW::LibraryAnimationClips(sw),
        depsgraph(depsgraph),
        sw(sw),
        export_settings(export_settings),
        anim_meta(anim_meta)
  {
  }

  void exportAnimationClips(blender::Scene *sce);
};
