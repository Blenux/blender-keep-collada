/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup collada
 */

#pragma once

#include <list>
#include <string>
// #include <vector>

#include "COLLADASWInstanceController.h"
#include "COLLADASWLibraryControllers.h"
#include "COLLADASWStreamWriter.h"

#include "DNA_armature_types.h"
#include "DNA_key_types.h"
#include "DNA_listBase.h"
#include "DNA_mesh_types.h"
#include "DNA_object_types.h"

#include "InstanceWriter.h"
#include "TransformWriter.h"

#include "ExportSettings.h"

class SceneExporter;

class ControllerExporter : public COLLADASW::LibraryControllers,
                           protected TransformWriter,
                           protected InstanceWriter {
 private:
  BlenderContext &blender_context;
  BCExportSettings export_settings;

 public:
  /* XXX exporter writes wrong data for shared armatures.  A separate
   * controller should be written for each armature-mesh binding how do
   * we make controller ids then? */
  ControllerExporter(BlenderContext &blender_context,
                     COLLADASW::StreamWriter *sw,
                     BCExportSettings &export_settings)
      : COLLADASW::LibraryControllers(sw),
        blender_context(blender_context),
        export_settings(export_settings)
  {
  }

  bool is_skinned_mesh(blender::Object *ob);

  bool add_instance_controller(blender::Object *ob);

  void export_controllers();

  void operator()(blender::Object *ob);

 private:
#if 0
  std::vector<blender::Object *> written_armatures;

  bool already_written(blender::Object *ob_arm);

  void wrote(blender::Object *ob_arm);

  void find_objects_using_armature(blender::Object *ob_arm, std::vector<blender::Object *> &objects, blender::Scene *sce);
#endif

  std::string get_controller_id(blender::Object *ob_arm, blender::Object *ob);

  std::string get_controller_id(blender::Key *key, blender::Object *ob);

  /** `ob` should be of type OB_MESH, both arguments are required. */
  void export_skin_controller(blender::Object *ob, blender::Object *ob_arm);

  void export_morph_controller(blender::Object *ob, blender::Key *key);

  void add_joints_element(const blender::ListBase *defbase,
                          const std::string &joints_source_id,
                          const std::string &inv_bind_mat_source_id);

  void add_bind_shape_mat(blender::Object *ob);

  std::string add_morph_targets(blender::Key *key, blender::Object *ob);

  std::string add_morph_weights(blender::Key *key, blender::Object *ob);

  /**
   * Added to implement support for animations.
   */
  void add_weight_extras(blender::Key *key);

  std::string add_joints_source(blender::Object *ob_arm,
                                const blender::ListBase *defbase,
                                const std::string &controller_id);

  std::string add_inv_bind_mats_source(blender::Object *ob_arm,
                                       const blender::ListBase *defbase,
                                       const std::string &controller_id);

  blender::Bone *get_bone_from_defgroup(blender::Object *ob_arm, const blender::bDeformGroup *def);

  bool is_bone_defgroup(blender::Object *ob_arm, const blender::bDeformGroup *def);

  std::string add_weights_source(blender::Mesh *mesh,
                                 const std::string &controller_id,
                                 const std::list<float> &weights);

  void add_vertex_weights_element(const std::string &weights_source_id,
                                  const std::string &joints_source_id,
                                  const std::list<int> &vcount,
                                  const std::list<int> &joints);

  void write_bone_URLs(COLLADASW::InstanceController &ins, blender::Object *ob_arm, blender::Bone *bone);
};
