/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup collada
 */

#pragma once

#include <string>
#include <vector>

#include "COLLADASWLibraryMaterials.h"
#include "COLLADASWStreamWriter.h"

#include "BKE_material.hh"

#include "DNA_material_types.h"
#include "DNA_object_types.h"
#include "DNA_scene_types.h"

#include "ExportSettings.h"
#include "GeometryExporter.h"
#include "collada_internal.h"

class MaterialsExporter : COLLADASW::LibraryMaterials {
 public:
  MaterialsExporter(COLLADASW::StreamWriter *sw, BCExportSettings &export_settings);
  void exportMaterials(blender::Scene *sce);
  void operator()(blender::Material *ma, blender::Object *ob);

 private:
  bool hasMaterials(blender::Scene *sce);
  BCExportSettings &export_settings;
};

/* Used in `forEachMaterialInScene`. */
template<class Functor> class ForEachMaterialFunctor {
  std::vector<std::string>
      mMat; /* contains list of material names, to avoid duplicate calling of f */
  Functor *f;

 public:
  ForEachMaterialFunctor(Functor *f) : f(f) {}

  void operator()(blender::Object *ob)
  {
    int a;
    for (a = 0; a < ob->totcol; a++) {

      blender::Material *ma = blender::BKE_object_material_get(ob, a + 1);

      if (!ma) {
        continue;
      }

      std::string translated_id = translate_id(id_name(ma));
      if (find(mMat.begin(), mMat.end(), translated_id) == mMat.end()) {
        (*this->f)(ma, ob);

        mMat.push_back(translated_id);
      }
    }
  }
};

struct MaterialFunctor {
  /* calls f for each unique material linked to each object in sce
   * f should have */
  // void operator()(Material *ma)
  template<class Functor>
  void forEachMaterialInExportSet(blender::Scene *sce, Functor &f, blender::LinkNode *export_set)
  {
    ForEachMaterialFunctor<Functor> matfunc(&f);
    GeometryFunctor gf;
    gf.forEachMeshObjectInExportSet<ForEachMaterialFunctor<Functor>>(sce, matfunc, export_set);
  }
};
