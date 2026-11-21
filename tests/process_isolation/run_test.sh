#!/usr/bin/env bash
# End to end test of process isolation: runs the fixture binary, built in an
# analysis mode, and checks the processes it used and its exit code.
set -u

FIXTURE="$1"
EXPECTED_BENCHMARKS=(BM_First BM_Second BM_OnMainWorker)
FAILING_EXIT_CODE=3
TIMEOUT_SECONDS=120

failures=0
fail() {
  echo "FAIL: $*" >&2
  failures=$((failures + 1))
}

run_fixture() {
  timeout "$TIMEOUT_SECONDS" "$FIXTURE" "$@" >"$OUTPUT_FILE" 2>"$ERROR_FILE"
  return $?
}

OUTPUT_FILE="$(mktemp)"
ERROR_FILE="$(mktemp)"
trap 'rm -f "$OUTPUT_FILE" "$ERROR_FILE"' EXIT

distinct_pids() {
  grep '^PID ' "$OUTPUT_FILE" | awk '{print $3}' | sort -u | wc -l
}

check_all_benchmarks_ran() {
  local label="$1"
  for name in "${EXPECTED_BENCHMARKS[@]}"; do
    grep -q "::${name}" "$ERROR_FILE" || fail "$label: no 'Measured/Checked' line for $name"
  done
}

# 1. Isolation on (default and explicit): one process per benchmark.
for setting in "" "true" "1"; do
  label="isolation=${setting:-<unset>}"
  if [ -z "$setting" ]; then
    env -u CODSPEED_PROCESS_ISOLATION -u FIXTURE_FAIL \
      timeout "$TIMEOUT_SECONDS" "$FIXTURE" >"$OUTPUT_FILE" 2>"$ERROR_FILE"
  else
    CODSPEED_PROCESS_ISOLATION="$setting" run_fixture
  fi
  [ $? -eq 0 ] || fail "$label: expected exit code 0"
  check_all_benchmarks_ran "$label"
  pids=$(distinct_pids)
  [ "$pids" -eq "${#EXPECTED_BENCHMARKS[@]}" ] ||
    fail "$label: expected ${#EXPECTED_BENCHMARKS[@]} distinct PIDs, got $pids"
  # The parent prints the context once, children must stay quiet.
  [ "$(grep -c 'Run on' "$ERROR_FILE")" -le 1 ] || fail "$label: context printed more than once"
done

# 2. Isolation off: all benchmarks share one process.
for setting in "false" "0"; do
  label="isolation=$setting"
  CODSPEED_PROCESS_ISOLATION="$setting" run_fixture
  [ $? -eq 0 ] || fail "$label: expected exit code 0"
  check_all_benchmarks_ran "$label"
  pids=$(distinct_pids)
  [ "$pids" -eq 1 ] || fail "$label: expected 1 PID, got $pids"
done

# 3. A failing benchmark fails the whole run, with or without isolation.
for setting in "true" "false"; do
  label="failing, isolation=$setting"
  FIXTURE_FAIL=1 CODSPEED_PROCESS_ISOLATION="$setting" run_fixture
  code=$?
  [ "$code" -eq "$FAILING_EXIT_CODE" ] ||
    fail "$label: expected exit code $FAILING_EXIT_CODE, got $code"
done

# 4. The child ignores the command line filter.
CODSPEED_PROCESS_ISOLATION=true run_fixture --benchmark_filter=BM_First
[ $? -eq 0 ] || fail "filter: expected exit code 0"
grep -q "::BM_First" "$ERROR_FILE" || fail "filter: BM_First did not run"
grep -q "::BM_Second" "$ERROR_FILE" && fail "filter: BM_Second ran despite the filter"

# 5. An invalid value is a hard error.
CODSPEED_PROCESS_ISOLATION=maybe run_fixture
[ $? -ne 0 ] || fail "invalid value: expected a non-zero exit code"
grep -q "CODSPEED_PROCESS_ISOLATION" "$ERROR_FILE" || fail "invalid value: no error message"

if [ "$failures" -ne 0 ]; then
  exit 1
fi
echo "process isolation test passed"
