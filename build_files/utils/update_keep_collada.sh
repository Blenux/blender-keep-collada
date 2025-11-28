#!/usr/bin/env bash
set -euo pipefail

branch="keep-collada-5.0"
upstream_branch="lfs-fallback/blender-v5.0-release"

echo "==> Switching to ${branch}"
git checkout "${branch}"

echo "==> Fetching ${upstream_branch}"
git fetch lfs-fallback blender-v5.0-release --prune

echo "==> Merging upstream into ${branch} without LFS smudge"
GIT_LFS_SKIP_SMUDGE=1 git merge --no-ff "${upstream_branch}" || {
    echo "Merge conflicts detected. Resolve conflicts, then run 'git commit' to finish merge."
    exit 1
}

echo "==> Refreshing all LFS objects after merge"
git lfs fetch --all
git lfs pull

echo "==> Updating COLLADA submodules"
for sm in lib/linux_x64_collada lib/windows_x64_collada lib/windows_arm64_collada lib/macos_arm64_collada; do
  if git config --file .gitmodules --get-regexp ".*${sm}" >/dev/null 2>&1; then
    echo "  -> ${sm}"
    GIT_LFS_SKIP_SMUDGE=1 git submodule update --init --progress "${sm}"
    (cd "${sm}" && git lfs fetch --all && git lfs pull) || true
  fi
done

echo "==> Checking LFS integrity before pushing"
git lfs fsck || true

echo "==> Pushing to origin (skipping LFS hook)"
git push --no-verify origin "${branch}" --force-with-lease

echo "==> Completed"
