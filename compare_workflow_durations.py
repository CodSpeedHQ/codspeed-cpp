#!/usr/bin/env python3
"""
Compare workflow durations between two branches.
"""

import json
import subprocess
import statistics
from datetime import datetime
from typing import List, Dict


def get_workflow_runs(branch: str, limit: int = 20) -> List[Dict]:
    """Fetch workflow runs for a specific branch."""
    cmd = [
        "gh", "run", "list",
        "--workflow", "ci.yml",
        "--branch", branch,
        "--limit", str(limit),
        "--json", "startedAt,updatedAt,status,conclusion,number"
    ]

    result = subprocess.run(cmd, capture_output=True, text=True, check=True)
    runs = json.loads(result.stdout)

    # Filter only completed runs and calculate duration
    completed_runs = []
    for run in runs:
        if run['status'] == 'completed' and run['startedAt'] and run['updatedAt']:
            started = datetime.fromisoformat(run['startedAt'].replace('Z', '+00:00'))
            updated = datetime.fromisoformat(run['updatedAt'].replace('Z', '+00:00'))
            duration_ms = (updated - started).total_seconds() * 1000
            run['durationMs'] = duration_ms
            completed_runs.append(run)

    return completed_runs


def calculate_stats(durations: List[float]) -> Dict[str, float]:
    """Calculate statistics for durations."""
    if not durations:
        return {"count": 0, "avg_seconds": 0, "median_seconds": 0, "avg_minutes": 0, "median_minutes": 0}

    avg_ms = statistics.mean(durations)
    median_ms = statistics.median(durations)

    return {
        "count": len(durations),
        "avg_seconds": avg_ms / 1000,
        "median_seconds": median_ms / 1000,
        "avg_minutes": avg_ms / 60000,
        "median_minutes": median_ms / 60000,
    }


def main():
    branches = ["test-action-no-cache", "test-action-cache-effects"]

    print("Fetching workflow runs...\n")

    results = {}
    for branch in branches:
        print(f"Fetching runs for branch: {branch}")
        runs = get_workflow_runs(branch, limit=20)
        durations = [run['durationMs'] for run in runs]
        stats = calculate_stats(durations)
        results[branch] = stats

        print(f"  Found {len(runs)} completed runs")

    # Display results
    print("\n" + "=" * 80)
    print("WORKFLOW DURATION COMPARISON")
    print("=" * 80 + "\n")

    for branch in branches:
        stats = results[branch]
        print(f"Branch: {branch}")
        print(f"  Completed runs: {stats['count']}")
        print(f"  Average:        {stats['avg_minutes']:.2f} minutes ({stats['avg_seconds']:.1f} seconds)")
        print(f"  Median:         {stats['median_minutes']:.2f} minutes ({stats['median_seconds']:.1f} seconds)")
        print()

    # Calculate difference
    if results[branches[0]]["count"] > 0 and results[branches[1]]["count"] > 0:
        print("=" * 80)
        print("COMPARISON")
        print("=" * 80 + "\n")

        cache_stats = results["test-action-cache-effects"]
        no_cache_stats = results["test-action-no-cache"]

        avg_diff = no_cache_stats["avg_seconds"] - cache_stats["avg_seconds"]
        median_diff = no_cache_stats["median_seconds"] - cache_stats["median_seconds"]

        avg_pct = (avg_diff / no_cache_stats["avg_seconds"]) * 100 if no_cache_stats["avg_seconds"] > 0 else 0
        median_pct = (median_diff / no_cache_stats["median_seconds"]) * 100 if no_cache_stats["median_seconds"] > 0 else 0

        print(f"Average difference:  {abs(avg_diff):.1f} seconds ({abs(avg_pct):.1f}%)")
        if avg_diff > 0:
            print(f"  → Cache makes workflow FASTER")
        else:
            print(f"  → Cache makes workflow SLOWER")

        print(f"\nMedian difference:   {abs(median_diff):.1f} seconds ({abs(median_pct):.1f}%)")
        if median_diff > 0:
            print(f"  → Cache makes workflow FASTER")
        else:
            print(f"  → Cache makes workflow SLOWER")


if __name__ == "__main__":
    main()
