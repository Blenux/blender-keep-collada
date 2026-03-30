/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include <map>
#include <string>

#include "BKE_context.hh"
#include "BKE_node.hh"
#include "DNA_material_types.h"
#include "DNA_node_types.h"

#include "COLLADAFWEffectCommon.h"
#include "collada_utils.h"

using NodeMap = std::map<std::string, blender::bNode *>;

class MaterialNode {

 private:
  blender::bContext *mContext;
  blender::Material *material;
  COLLADAFW::EffectCommon *effect;
  UidImageMap *uid_image_map = nullptr;
  KeyImageMap *key_image_map = nullptr;

  NodeMap node_map;
  blender::bNodeTree *ntree;

  blender::bNode *shader_node;
  blender::bNode *output_node;

  /** Returns null if material already has a node tree. */
  blender::bNodeTree *prepare_material_nodetree();
  blender::bNode *add_node(int node_type, int locx, int locy, std::string label);
  void add_link(blender::bNode *from_node, int from_index, blender::bNode *to_node, int to_index);
  void add_link(blender::bNode *from_node, const char *from_label, blender::bNode *to_node, const char *to_label);
  blender::bNode *add_texture_node(COLLADAFW::ColorOrTexture &cot, int locx, int locy, std::string label);
  void setShaderType();

 public:
  MaterialNode(blender::bContext *C, COLLADAFW::EffectCommon *ef, blender::Material *ma, UidImageMap &uid_image_map);
  MaterialNode(blender::bContext *C, blender::Material *ma, KeyImageMap &key_image_map);
  blender::Image *get_diffuse_image();

  void set_diffuse(COLLADAFW::ColorOrTexture &cot);
  void set_specular(COLLADAFW::ColorOrTexture &cot);
  void set_ambient(COLLADAFW::ColorOrTexture &cot);
  void set_reflective(COLLADAFW::ColorOrTexture &cot);
  void set_emission(COLLADAFW::ColorOrTexture &cot);
  void set_opacity(COLLADAFW::ColorOrTexture &cot);
  void set_reflectivity(COLLADAFW::FloatOrParam &val);
  void set_shininess(COLLADAFW::FloatOrParam &val);
  void set_ior(COLLADAFW::FloatOrParam &val);
  void set_alpha(COLLADAFW::EffectCommon::OpaqueMode mode,
                 COLLADAFW::ColorOrTexture &cot,
                 COLLADAFW::FloatOrParam &val);

  void update_material_nodetree();
};
