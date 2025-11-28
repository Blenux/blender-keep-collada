#!/usr/bin/env bash
set -euo pipefail

branch="keep-collada-5.0"

echo "==> Switching to ${branch}"
git checkout "${branch}"

echo "==> Fetching lfs-fallback/blender-v5.0-release"
git fetch lfs-fallback blender-v5.0-release --prune

echo "==> Rebasing without LFS smudge"
GIT_LFS_SKIP_SMUDGE=1 git rebase lfs-fallback/blender-v5.0-release

echo "==> Refreshing all LFS objects after rebase"
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
