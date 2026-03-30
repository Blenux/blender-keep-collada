/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup collada
 */

#pragma once

#include <string>

#include "COLLADASWColorOrTexture.h"
#include "COLLADASWLibraryEffects.h"
#include "COLLADASWSampler.h"
#include "COLLADASWStreamWriter.h"

#include "DNA_image_types.h"
#include "DNA_material_types.h"
#include "DNA_object_types.h"
#include "DNA_scene_types.h"

#include "ExportSettings.h"
#include "collada_utils.h"

class EffectsExporter : COLLADASW::LibraryEffects {
 public:
  EffectsExporter(COLLADASW::StreamWriter *sw,
                  BCExportSettings &export_settings,
                  KeyImageMap &key_image_map);
  void exportEffects(blender::bContext *C, blender::Scene *sce);

  void operator()(blender::Material *ma, blender::Object *ob);

  COLLADASW::ColorOrTexture createTexture(blender::Image *ima,
                                          std::string &uv_layer_name,
                                          COLLADASW::Sampler *sampler
                                          /*COLLADASW::Surface *surface*/);

  COLLADASW::ColorOrTexture getcol(float r, float g, float b, float a);

 private:
  void set_shader_type(COLLADASW::EffectProfile &ep, blender::Material *ma);

  void set_diffuse_color(COLLADASW::EffectProfile &ep, blender::Material *ma);
  void set_emission(COLLADASW::EffectProfile &ep, blender::Material *ma);
  void set_ior(COLLADASW::EffectProfile &ep, blender::Material *ma);
  void set_shininess(COLLADASW::EffectProfile &ep, blender::Material *ma);
  void set_reflectivity(COLLADASW::EffectProfile &ep, blender::Material *ma);
  void set_transparency(COLLADASW::EffectProfile &ep, blender::Material *ma);
  void set_ambient(COLLADASW::EffectProfile &ep, blender::Material *ma);
  void set_specular(COLLADASW::EffectProfile &ep, blender::Material *ma);
  void set_reflective(COLLADASW::EffectProfile &ep, blender::Material *ma);

  void get_images(blender::Material *ma, KeyImageMap &material_image_map);
  void create_image_samplers(COLLADASW::EffectProfile &ep,
                             KeyImageMap &material_image_map,
                             std::string &active_uv);

  void writeTextures(COLLADASW::EffectProfile &ep,
                     std::string &key,
                     COLLADASW::Sampler *sampler,
                     blender::MTex *t,
                     blender::Image *ima,
                     std::string &uvname);

  bool hasEffects(blender::Scene *sce);

  BCExportSettings &export_settings;
  KeyImageMap &key_image_map;
  blender::Scene *scene;
  blender::bContext *mContext;
};
