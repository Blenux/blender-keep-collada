/* SPDX-FileCopyrightText: 2002-2022 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup collada
 */

#include "MaterialExporter.h"
#include "COLLADABUUtils.h"
#include "collada_internal.h"

using namespace blender;

MaterialsExporter::MaterialsExporter(COLLADASW::StreamWriter *sw,
                                     BCExportSettings &export_settings)
    : COLLADASW::LibraryMaterials(sw), export_settings(export_settings)
{
  /* pass */
}

void MaterialsExporter::exportMaterials(blender::Scene *sce)
{
  if (hasMaterials(sce)) {
    openLibrary();

    MaterialFunctor mf;
    mf.forEachMaterialInExportSet<MaterialsExporter>(
        sce, *this, this->export_settings.get_export_set());

    closeLibrary();
  }
}

bool MaterialsExporter::hasMaterials(blender::Scene *sce)
{
  blender::LinkNode *node;
  for (node = this->export_settings.get_export_set(); node; node = node->next) {
    blender::Object *ob = (blender::Object *)node->link;
    int a;
    for (a = 0; a < ob->totcol; a++) {
      blender::Material *ma = BKE_object_material_get(ob, a + 1);

      /* no material, but check all of the slots */
      if (!ma) {
        continue;
      }

      return true;
    }
  }
  return false;
}

void MaterialsExporter::operator()(blender::Material *ma, blender::Object *ob)
{
  std::string mat_name = encode_xml(id_name(ma));
  std::string mat_id = get_material_id(ma);
  std::string eff_id = get_effect_id(ma);

  openMaterial(mat_id, mat_name);
  addInstanceEffect(COLLADASW::URI(COLLADABU::Utils::EMPTY_STRING, eff_id));

  closeMaterial();
}
