/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup collada
 */

#pragma once

#include "BKE_context.hh"
#include "BKE_main.hh"
#include "BLI_linklist.h"
#include "BlenderTypes.h"
#include "DEG_depsgraph.hh"
#include "DEG_depsgraph_query.hh"
#include "DNA_layer_types.h"
#include "DNA_object_types.h"

static const BC_global_forward_axis BC_DEFAULT_FORWARD = BC_GLOBAL_FORWARD_Y;
static const BC_global_up_axis BC_DEFAULT_UP = BC_GLOBAL_UP_Z;

bool bc_is_in_Export_set(blender::LinkNode *export_set,
                         blender::Object *ob,
                         const blender::Scene *scene,
                         blender::ViewLayer *view_layer);
bool bc_is_base_node(blender::LinkNode *export_set, blender::Object *ob, const blender::Scene *scene, blender::ViewLayer *view_layer);
/**
 * Returns the highest selected ancestor
 * returns NULL if no ancestor is selected
 * IMPORTANT: This function expects that all exported objects have set:
 * `ob->id.tag & ID_TAG_DOIT`
 */
blender::Object *bc_get_highest_exported_ancestor_or_self(blender::LinkNode *export_set,
                                                 blender::Object *ob,
                                                 const blender::Scene *scene,
                                                 blender::ViewLayer *view_layer);
int bc_is_marked(blender::Object *ob);
void bc_remove_mark(blender::Object *ob);
void bc_set_mark(blender::Object *ob);

class BlenderContext {
 private:
  blender::bContext *context;
  blender::Depsgraph *depsgraph;
  blender::Scene *scene;
  blender::ViewLayer *view_layer;
  blender::Main *main;

 public:
  BlenderContext(blender::bContext *C);
  blender::bContext *get_context();
  blender::Depsgraph *get_depsgraph();
  blender::Scene *get_scene();
  blender::Scene *get_evaluated_scene();
  blender::Object *get_evaluated_object(blender::Object *ob);
  blender::ViewLayer *get_view_layer();
  blender::Main *get_main();
};
