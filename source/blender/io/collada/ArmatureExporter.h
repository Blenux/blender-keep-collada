/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup collada
 */

#pragma once

#include <string>

#include <COLLADASWInputList.h>
#include <COLLADASWInstanceController.h>
#include <COLLADASWLibraryControllers.h>
#include <COLLADASWNode.h>
#include <COLLADASWStreamWriter.h>

#include "DNA_armature_types.h"
#include "DNA_object_types.h"

#include "InstanceWriter.h"
#include "TransformWriter.h"

#include "ExportSettings.h"

class SceneExporter;

/* XXX exporter writes wrong data for shared armatures.  A separate
 * controller should be written for each armature-mesh binding how do
 * we make controller ids then? */
class ArmatureExporter : public COLLADASW::LibraryControllers,
                         protected TransformWriter,
                         protected InstanceWriter {
 public:
  /* XXX exporter writes wrong data for shared armatures.  A separate
   * controller should be written for each armature-mesh binding how do
   * we make controller ids then? */
  ArmatureExporter(BlenderContext &blender_context,
                   COLLADASW::StreamWriter *sw,
                   BCExportSettings &export_settings)
      : COLLADASW::LibraryControllers(sw),
        blender_context(blender_context),
        export_settings(export_settings)
  {
  }

  void add_bone_collections(blender::Object *ob_arm, COLLADASW::Node &node);

  /* write bone nodes */
  void add_armature_bones(blender::Object *ob_arm,
                          blender::ViewLayer *view_layer,
                          SceneExporter *se,
                          std::vector<blender::Object *> &child_objects);

  bool add_instance_controller(blender::Object *ob);

 private:
  BlenderContext &blender_context;
  BCExportSettings &export_settings;

#if 0
  std::vector<blender::Object *> written_armatures;

  bool already_written(blender::Object *ob_arm);

  void wrote(blender::Object *ob_arm);

  void find_objects_using_armature(blender::Object *ob_arm, std::vector<blender::Object *> &objects, blender::Scene *sce);
#endif

  /**
   * Scene, SceneExporter and the list of child_objects
   * are required for writing bone parented objects.
   * \param parent_mat: is armature-space.
   */
  void add_bone_node(blender::Bone *bone,
                     blender::Object *ob_arm,
                     SceneExporter *se,
                     std::vector<blender::Object *> &child_objects);

  bool can_export(blender::Bone *bone)
  {
    return !(export_settings.get_deform_bones_only() && bone->flag & blender::BONE_NO_DEFORM);
  }

  bool is_export_root(blender::Bone *bone);
  void add_bone_transform(blender::Object *ob_arm, blender::Bone *bone, COLLADASW::Node &node);

  std::string get_controller_id(blender::Object *ob_arm, blender::Object *ob);

  void write_bone_URLs(COLLADASW::InstanceController &ins, blender::Object *ob_arm, blender::Bone *bone);
};
