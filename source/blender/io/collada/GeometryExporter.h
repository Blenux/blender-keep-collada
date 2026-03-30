/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup collada
 */

#pragma once

#include <set>
#include <string>
#include <vector>

#include "COLLADASWInputList.h"
#include "COLLADASWLibraryGeometries.h"
#include "COLLADASWStreamWriter.h"

#include "DNA_key_types.h"
#include "DNA_mesh_types.h"
#include "DNA_object_types.h"
#include "DNA_scene_types.h"

#include "BlenderContext.h"
#include "ExportSettings.h"
#include "collada_utils.h"

class Normal {
 public:
  float x;
  float y;
  float z;

  friend bool operator<(const Normal &, const Normal &);
};

bool operator<(const Normal &, const Normal &);

/* TODO: optimize UV sets by making indexed list with duplicates removed */
class GeometryExporter : COLLADASW::LibraryGeometries {
  struct Face {
    unsigned int v1, v2, v3, v4;
  };

 public:
  /* TODO: optimize UV sets by making indexed list with duplicates removed */
  GeometryExporter(BlenderContext &blender_context,
                   COLLADASW::StreamWriter *sw,
                   BCExportSettings &export_settings)
      : COLLADASW::LibraryGeometries(sw),
        blender_context(blender_context),
        export_settings(export_settings)
  {
  }

  void exportGeom();

  void operator()(blender::Object *ob);

  void createLooseEdgeList(blender::Object *ob, blender::Mesh *mesh, std::string &geom_id);

  /** Powerful because it handles both cases when there is material and when there's not. */
  void create_mesh_primitive_list(short material_index,
                                  bool has_uvs,
                                  bool has_color,
                                  blender::Object *ob,
                                  blender::Mesh *mesh,
                                  std::string &geom_id,
                                  std::vector<BCPolygonNormalsIndices> &norind);

  /** Creates <source> for positions. */
  void createVertsSource(std::string geom_id, blender::Mesh *mesh);

  void createVertexColorSource(std::string geom_id, blender::Mesh *mesh);

  std::string makeTexcoordSourceId(std::string &geom_id, int layer_index, bool is_single_layer);

  /** Creates <source> for texture-coordinates. */
  void createTexcoordsSource(std::string geom_id, blender::Mesh *mesh);

  /** Creates <source> for normals. */
  void createNormalsSource(std::string geom_id, blender::Mesh *mesh, std::vector<Normal> &nor);

  void create_normals(std::vector<Normal> &nor,
                      std::vector<BCPolygonNormalsIndices> &polygons_normals,
                      blender::Mesh *mesh);

  std::string getIdBySemantics(std::string geom_id,
                               COLLADASW::InputSemantic::Semantics type,
                               std::string other_suffix = "");
  std::string makeVertexColorSourceId(std::string &geom_id, const char *layer_name);

  COLLADASW::URI getUrlBySemantics(std::string geom_id,
                                   COLLADASW::InputSemantic::Semantics type,
                                   std::string other_suffix = "");

  COLLADASW::URI makeUrl(std::string id);

  void export_key_mesh(blender::Object *ob, blender::Mesh *mesh, blender::KeyBlock *kb);

 private:
  std::set<std::string> exportedGeometry;
  BlenderContext &blender_context;
  BCExportSettings &export_settings;

  blender::Mesh *get_mesh(blender::Scene *sce, blender::Object *ob, int apply_modifiers);
};

struct GeometryFunctor {
  /* f should have
   * void operator()(blender::Object *ob) */
  template<class Functor>
  void forEachMeshObjectInExportSet(blender::Scene *sce, Functor &f, blender::LinkNode *export_set)
  {
    blender::LinkNode *node;
    for (node = export_set; node; node = node->next) {
      blender::Object *ob = (blender::Object *)node->link;
      if (ob->type == blender::OB_MESH) {
        f(ob);
      }
    }
  }
};
