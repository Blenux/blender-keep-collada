#!/usr/bin/env bash
set -euo pipefail

########################################
# Configuration
########################################

branch="keep-collada-5.0"
remote="origin"
upstream_remote="lfs-fallback"
upstream_branch="blender-v5.0-release"

collada_submodules=(
  lib/linux_x64_collada
  lib/windows_x64_collada
  lib/windows_arm64_collada
  lib/macos_arm64_collada
)

########################################
# Helpers
########################################

die() {
  echo "ERROR: $*" >&2
  exit 1
}

info() {
  echo "==> $*"
}

require_clean_tree() {
  if ! git diff --quiet || ! git diff --cached --quiet; then
    die "Working tree is dirty. Commit or stash changes first."
  fi
}

########################################
# Preconditions
########################################

info "Checking Git LFS"
command -v git-lfs >/dev/null 2>&1 || die "git-lfs not installed"

require_clean_tree

########################################
# Checkout target branch
########################################

info "Checking out ${branch}"
git checkout "${branch}"

########################################
# Fetch upstream safely
########################################

info "Fetching upstream from ${upstream_remote}"
git fetch "${upstream_remote}" --prune

upstream_ref="${upstream_remote}/${upstream_branch}"
git show-ref --verify --quiet "refs/remotes/${upstream_ref}" \
  || die "Upstream ref ${upstream_ref} not found"

########################################
# Merge without LFS smudge
########################################

info "Merging ${upstream_ref} into ${branch} (LFS smudge disabled)"
GIT_LFS_SKIP_SMUDGE=1 git merge --no-ff "${upstream_ref}" || {
  echo
  echo "Merge conflicts detected."
  echo "Resolve conflicts, then run:"
  echo "  git commit"
  exit 1
}

########################################
# Ensure all LFS objects exist locally
########################################

info "Verifying local LFS objects"
git lfs fsck || die "Missing local LFS objects – cannot continue"

########################################
# Push ALL LFS objects (CRITICAL STEP)
########################################

info "Pushing all LFS objects to ${remote}"
git lfs push --all "${remote}" "${branch}"

########################################
# Update COLLADA submodules
########################################

info "Updating COLLADA submodules"

for sm in "${collada_submodules[@]}"; do
  if git config --file .gitmodules --get-regexp ".*${sm}" >/dev/null 2>&1; then
    info "  -> ${sm}"

    GIT_LFS_SKIP_SMUDGE=1 git submodule update --init --progress "${sm}"

    (
      cd "${sm}"
      git lfs fetch --all
      git lfs pull
      git lfs fsck || die "LFS integrity failure in submodule ${sm}"
    )
  fi
done

########################################
# Final push (safe force)
########################################

info "Pushing branch ${branch} to ${remote}"
git push "${remote}" "${branch}" --force-with-lease

########################################
# Final verification hint
########################################

echo
info "Completed successfully"
echo
echo "Recommended final check:"
echo "  git clone <repo-url> /tmp/test && cd /tmp/test && git lfs pull"
echo
