/* SPDX-FileCopyrightText: 2009-2022 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup collada
 */

#include <vector>

#include "BlenderContext.h"
#include "ExportSettings.h"

#include "BKE_layer.hh"
#include "BKE_scene.hh"

#include "BLI_listbase.h"

using namespace blender;

bool bc_is_base_node(blender::LinkNode *export_set, blender::Object *ob, const blender::Scene *scene, blender::ViewLayer *view_layer)
{
  blender::Object *root = bc_get_highest_exported_ancestor_or_self(export_set, ob, scene, view_layer);
  return (root == ob);
}

blender::Object *bc_get_highest_exported_ancestor_or_self(blender::LinkNode *export_set,
                                                 blender::Object *ob,
                                                 const blender::Scene *scene,
                                                 blender::ViewLayer *view_layer)
{
  blender::Object *ancestor = ob;
  while (ob->parent) {
    if (bc_is_in_Export_set(export_set, ob->parent, scene, view_layer)) {
      ancestor = ob->parent;
    }
    ob = ob->parent;
  }
  return ancestor;
}

void bc_get_children(std::vector<blender::Object *> &child_set,
                     blender::Object *ob,
                     const blender::Scene *scene,
                     blender::ViewLayer *view_layer)
{
  BKE_view_layer_synced_ensure(scene, view_layer);
  blender::ListBase *bases = BKE_view_layer_object_bases_get(view_layer);
  for (blender::Link *link = static_cast<blender::Link*>(bases->first); 
       link != nullptr; 
       link = static_cast<blender::Link*>(link->next)) {
    blender::Base *base = reinterpret_cast<blender::Base*>(link);
    blender::Object *cob = base->object;
    if (cob->parent == ob) {
      switch (ob->type) {
        case blender::OB_MESH:
        case blender::OB_CAMERA:
        case blender::OB_LAMP:
        case blender::OB_EMPTY:
        case blender::OB_ARMATURE:
          child_set.push_back(cob);
        default:
          break;
      }
    }
  }
}

bool bc_is_in_Export_set(blender::LinkNode *export_set,
                         blender::Object *ob,
                         const blender::Scene *scene,
                         blender::ViewLayer *view_layer)
{
  bool to_export = (BLI_linklist_index(export_set, ob) != -1);

  if (!to_export) {
    /* Mark this object as to_export even if it is not in the
     * export list, but it contains children to export. */

    std::vector<blender::Object *> children;
    bc_get_children(children, ob, scene, view_layer);
    for (blender::Object *child : children) {
      if (bc_is_in_Export_set(export_set, child, scene, view_layer)) {
        to_export = true;
        break;
      }
    }
  }
  return to_export;
}

int bc_is_marked(blender::Object *ob)
{
  return ob && (ob->id.tag & blender::ID_TAG_DOIT);
}

void bc_remove_mark(blender::Object *ob)
{
  ob->id.tag &= ~blender::ID_TAG_DOIT;
}

void bc_set_mark(blender::Object *ob)
{
  ob->id.tag |= blender::ID_TAG_DOIT;
}

BlenderContext::BlenderContext(blender::bContext *C)
{
  context = C;
  main = CTX_data_main(C);
  scene = CTX_data_scene(C);
  view_layer = CTX_data_view_layer(C);
  depsgraph = nullptr; /* create only when needed */
}

blender::bContext *BlenderContext::get_context()
{
  return context;
}

blender::Depsgraph *BlenderContext::get_depsgraph()
{
  if (!depsgraph) {
    depsgraph = BKE_scene_ensure_depsgraph(main, scene, view_layer);
  }
  return depsgraph;
}

blender::Scene *BlenderContext::get_scene()
{
  return scene;
}

blender::Scene *BlenderContext::get_evaluated_scene()
{
  blender::Scene *scene_eval = DEG_get_evaluated_scene(get_depsgraph());
  return scene_eval;
}

blender::Object *BlenderContext::get_evaluated_object(blender::Object *ob)
{
  blender::Object *ob_eval = DEG_get_evaluated(depsgraph, ob);
  return ob_eval;
}

blender::ViewLayer *BlenderContext::get_view_layer()
{
  return view_layer;
}

blender::Main *BlenderContext::get_main()
{
  return main;
}
