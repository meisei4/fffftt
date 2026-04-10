#!/usr/bin/env bash
set -euo pipefail

project_root="$(cd "$(dirname "$0")" && pwd)"
log_directory="${1:-$project_root/logs}"
tmux_session_prefix="${2:-audio-only}"
tmux_start_gate="${tmux_session_prefix}-start-$$"
capture_seconds="${TMUX_AUDIO_ONLY_CAPTURE_SECONDS:-15}"
any_build_failed=0
successful_targets=()
result_lines=()
period_1024_flag='ALT_AUDIO_DEVICE_PERIOD_FRAMES_KOS_CFLAGS=KOS_CFLAGS="$(KOS_CFLAGS) -DAUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES=1024"'

targets=(
  "audio-only-wav-dc-2048|wav|2048|$project_root/bin/audio_only_wav_dc_2048|$log_directory/audio-only-wav-dc-2048.log"
  "audio-only-wav-dc-1024|wav|1024|$project_root/bin/audio_only_wav_dc_1024|$log_directory/audio-only-wav-dc-1024.log"
  "audio-only-mp3-dc-2048|mp3|2048|$project_root/bin/audio_only_mp3_dc_2048|$log_directory/audio-only-mp3-dc-2048.log"
  "audio-only-mp3-dc-1024|mp3|1024|$project_root/bin/audio_only_mp3_dc_1024|$log_directory/audio-only-mp3-dc-1024.log"
)

mkdir -p "$log_directory"
rm -f "$log_directory"/*.log

cleanup() {
  for entry in "${targets[@]}"; do
    name="${entry%%|*}"
    tmux kill-session -t "${tmux_session_prefix}-${name}" 2>/dev/null || true
  done
}
trap cleanup EXIT

for entry in "${targets[@]}"; do
  IFS='|' read -r name target period launcher_path log_file <<< "$entry"
  build_dir="$project_root/build/dc/audio_only_${target}_dc_${period}"
  if [ "$target" = "mp3" ]; then
    elf_path="$project_root/build/dc/audio_only_mp3_dc/audio_only_mp3_dc.elf"
  else
    elf_path="$project_root/build/dc/audio_only_wav_dc/audio_only_wav_dc.elf"
  fi
  session_name="${tmux_session_prefix}-${name}"

  if [ "$target" = "mp3" ]; then
    make_args=(make -C "$project_root" audio-only-mp3-dc)
  else
    make_args=(make -C "$project_root" audio-only-wav-dc)
  fi

  if [ "$period" = "1024" ]; then
    make_args+=("$period_1024_flag")
  fi

  if "${make_args[@]}"; then
    mkdir -p "$build_dir" "$project_root/bin"
    cp "$elf_path" "$build_dir/"
    if [ "$target" = "mp3" ]; then
      target_elf="$build_dir/audio_only_mp3_dc.elf"
    else
      target_elf="$build_dir/audio_only_wav_dc.elf"
    fi
    printf '%s\n' '#!/usr/bin/env bash' 'set -euo pipefail' "d=\"\$(cd \"\$(dirname \"\$0\")/..\" && pwd)\"" "exec \"\$d/flycast_run.sh\" \"\$d/build/dc/audio_only_${target}_dc_${period}/audio_only_${target}_dc.elf\" \"\$@\"" > "$launcher_path"
    chmod +x "$launcher_path"
    result_lines+=("$(printf '%-24s [SUCCESS]' "$name")")
    successful_targets+=("$name")
  else
    result_lines+=("$(printf '%-24s [FAILED]' "$name")")
    any_build_failed=1
    continue
  fi

  tmux kill-session -t "$session_name" 2>/dev/null || true
  tmux new-session -d -s "$session_name" \
    "tmux wait-for '$tmux_start_gate' && cd '$project_root' && FLYCAST_LOG_PATH=/dev/null '$launcher_path' >'$log_file' 2>&1"
done

printf '\n'
printf '########################################\n'
printf '##   TMUX_AUDIO_ONLY_MATRIX RESULTS   ##\n'
printf '########################################\n'
for result_line in "${result_lines[@]}"; do
  printf '%s\n' "$result_line"
done
printf '\n'

if [ "${#successful_targets[@]}" -eq 0 ]; then
  exit 1
fi

sleep 1
tmux wait-for -S "$tmux_start_gate"
sleep "$capture_seconds"

exit "$any_build_failed"
