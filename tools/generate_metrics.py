#!/usr/bin/env python3
"""
Parse simulator log and produce a JSON metrics file for the frontend.

Usage: python3 tools/generate_metrics.py <logfile> <out_json>
"""
import sys
import re
import json

def parse(logpath):
    metrics = []
    assigns = []
    scales = []
    t_re = re.compile(r"t=(\d+) \| servers=(\d+) \| queued=(\d+)")
    assign_re = re.compile(r"\[assign\] server=(\d+) req=(\d+) ip_in=([^ ]+) ip_out=([^ ]+) cycles=(\d+)")
    scale_re = re.compile(r"\[scale\] (Added|Removed) server (\d+)")
    with open(logpath, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            m = t_re.search(line)
            if m:
                t = int(m.group(1))
                servers = int(m.group(2))
                queued = int(m.group(3))
                metrics.append({'t': t, 'servers': servers, 'queued': queued})
                continue
            m = assign_re.search(line)
            if m:
                assigns.append({'server': int(m.group(1)), 'req': int(m.group(2)), 'ip_in': m.group(3), 'ip_out': m.group(4), 'cycles': int(m.group(5))})
                continue
            m = scale_re.search(line)
            if m:
                scales.append({'action': m.group(1), 'server': int(m.group(2))})
    return {'metrics': metrics, 'assigns': assigns, 'scales': scales}

def main():
    if len(sys.argv) < 3:
        print('Usage: {} <logfile> <out_json>'.format(sys.argv[0]), file=sys.stderr)
        sys.exit(2)
    logpath = sys.argv[1]
    outpath = sys.argv[2]
    data = parse(logpath)
    with open(outpath, 'w', encoding='utf-8') as out:
        json.dump(data, out, indent=2)
    print('Wrote', outpath)

if __name__ == '__main__':
    main()
