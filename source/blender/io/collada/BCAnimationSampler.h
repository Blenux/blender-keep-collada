/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include "BCAnimationCurve.h"
#include "BCSampleData.h"
#include "collada_utils.h"

#include "BKE_action.hh"
#include "BKE_lib_id.hh"

/* Collection of animation curves */
class BCAnimation {
 private:
  blender::Object *reference = nullptr;
  blender::bContext *mContext;

 public:
  BCFrameSet frame_set;
  BCAnimationCurveMap curve_map;

  BCAnimation(blender::bContext *C, blender::Object *ob) : mContext(C)
  {
    blender::Main *bmain = blender::CTX_data_main(mContext);
    reference = (blender::Object *)blender::BKE_id_copy(bmain, &ob->id);
    id_us_min(&reference->id);
  }

  ~BCAnimation()
  {
    BCAnimationCurveMap::iterator it;
    for (it = curve_map.begin(); it != curve_map.end(); ++it) {
      delete it->second;
    }

    if (reference && reference->id.us == 0) {
      blender::Main *bmain = blender::CTX_data_main(mContext);
      BKE_id_delete(bmain, &reference->id);
    }
    curve_map.clear();
  }

  blender::Object *get_reference()
  {
    return reference;
  }
};

using BCAnimationObjectMap = std::map<blender::Object *, BCAnimation *>;

class BCSampleFrame {

  /* Each frame on the timeline that needs to be sampled will have
   * one BCSampleFrame where we collect sample information about all objects
   * that need to be sampled for that frame. */

 private:
  BCSampleMap sampleMap;

 public:
  ~BCSampleFrame()
  {
    BCSampleMap::iterator it;
    for (it = sampleMap.begin(); it != sampleMap.end(); ++it) {
      BCSample *sample = it->second;
      delete sample;
    }
    sampleMap.clear();
  }

  BCSample &add(blender::Object *ob);

  /* Following methods return NULL if object is not in the sampleMap. */

  /** Get the matrix for the given key, returns Unity when the key does not exist. */
  const BCSample *get_sample(blender::Object *ob) const;
  const BCMatrix *get_sample_matrix(blender::Object *ob) const;
  /** Get the matrix for the given Bone, returns Unity when the Object is not sampled. */
  const BCMatrix *get_sample_matrix(blender::Object *ob, blender::Bone *bone) const;

  /** Check if the key is in this BCSampleFrame. */
  bool has_sample_for(blender::Object *ob) const;
  /** Check if the Bone is in this BCSampleFrame. */
  bool has_sample_for(blender::Object *ob, blender::Bone *bone) const;
};

using BCSampleFrameMap = std::map<int, BCSampleFrame>;

class BCSampleFrameContainer {

  /*
   * The BCSampleFrameContainer stores a map of BCSampleFrame objects
   * with the timeline frame as key.
   *
   * Some details on the purpose:
   * An Animation is made of multiple FCurves where each FCurve can
   * have multiple keyframes. When we want to export the animation we
   * also can decide whether we want to export the keyframes or a set
   * of sample frames at equidistant locations (sample period).
   * In any case we must resample first need to resample it fully
   * to resolve things like:
   *
   * - animations by constraints
   * - animations by drivers
   *
   * For this purpose we need to step through the entire animation and
   * then sample each frame that contains at least one keyFrame or
   * sampleFrame. Then for each frame we have to store the transform
   * information for all exported objects in a BCSampleframe
   *
   * The entire set of BCSampleframes is finally collected into
   * a BCSampleframneContainer
   */

 private:
  BCSampleFrameMap sample_frames;

 public:
  ~BCSampleFrameContainer() = default;

  BCSample &add(blender::Object *ob, int frame_index);
  /** Return either the #BCSampleFrame or NULL if frame does not exist. */
  BCSampleFrame *get_frame(int frame_index);

  /** Return a list of all frames that need to be sampled. */
  int get_frames(std::vector<int> &frames) const;
  int get_frames(blender::Object *ob, BCFrames &frames) const;
  int get_frames(blender::Object *ob, blender::Bone *bone, BCFrames &frames) const;

  int get_samples(blender::Object *ob, BCFrameSampleMap &samples) const;
  int get_matrices(blender::Object *ob, BCMatrixSampleMap &samples) const;
  int get_matrices(blender::Object *ob, blender::Bone *bone, BCMatrixSampleMap &samples) const;
};

class BCAnimationSampler {
 private:
  BCExportSettings &export_settings;
  BCSampleFrameContainer sample_data;
  BCAnimationObjectMap objects;

  void generate_transform(blender::Object *ob, const BCCurveKey &key, BCAnimationCurveMap &curves);
  void generate_transforms(blender::Object *ob,
                           const std::string prep,
                           const BC_animation_type type,
                           BCAnimationCurveMap &curves);
  void generate_transforms(blender::Object *ob, blender::Bone *bone, BCAnimationCurveMap &curves);

  void initialize_curves(BCAnimationCurveMap &curves, blender::Object *ob);
  /**
   * Collect all keyframes from all animation curves related to the object.
   * The bc_get... functions check for NULL and correct object type.
   * The #add_keyframes_from() function checks for NULL.
   */
  void initialize_keyframes(BCFrameSet &frameset, blender::Object *ob);
  BCSample &sample_object(blender::Object *ob, int frame_index, bool for_opensim);
  void update_animation_curves(BCAnimation &animation,
                               BCSample &sample,
                               blender::Object *ob,
                               int frame_index);
  void check_property_is_animated(
      BCAnimation &animation, float *ref, float *val, std::string data_path, int length);

 public:
  BCAnimationSampler(BCExportSettings &export_settings, BCObjectSet &object_set);
  ~BCAnimationSampler();

  void add_object(blender::Object *ob);

  void sample_scene(BCExportSettings &export_settings, bool keyframe_at_end);

  BCAnimationCurveMap *get_curves(blender::Object *ob);
  void get_object_frames(BCFrames &frames, blender::Object *ob);
  bool get_object_samples(BCMatrixSampleMap &samples, blender::Object *ob);
  void get_bone_frames(BCFrames &frames, blender::Object *ob, blender::Bone *bone);
  bool get_bone_samples(BCMatrixSampleMap &samples, blender::Object *ob, blender::Bone *bone);

  static void get_animated_from_export_set(std::set<blender::Object *> &animated_objects,
                                           blender::LinkNode &export_set);
  static void find_depending_animated(std::set<blender::Object *> &animated_objects,
                                      std::set<blender::Object *> &candidates);
  static bool is_animated_by_constraint(blender::Object *ob,
                                        blender::ListBase *conlist,
                                        std::set<blender::Object *> &animated_objects);
};
