#!/usr/bin/env python3
"""
Live server: runs the simulator (or tails a log) and streams metrics via SSE.

Usage:
  python3 tools/live_server.py --servers 3 --duration 100 --port 8000

Clients can connect to http://localhost:8000/ for the frontend and
subscribe to SSE at /events to receive incremental JSON updates.
"""
import argparse
import http.server
import json
import queue
import re
import socketserver
import subprocess
import threading
import time
from typing import List
import os

CLIENTS: List[queue.Queue] = []

metrics = { 'metrics': [], 'assigns': [], 'scales': [] }
metrics_lock = threading.Lock()

T_RE = re.compile(r"t=(\d+) \| servers=(\d+) \| queued=(\d+)")
ASSIGN_RE = re.compile(r"\[assign\] server=(\d+) req=(\d+) ip_in=([^ ]+) ip_out=([^ ]+) cycles=(\d+)")
SCALE_RE = re.compile(r"\[scale\] (Added|Removed) server (\d+)")

def broadcast_update():
    data = json.dumps(metrics)
    for q in list(CLIENTS):
        try:
            q.put_nowait(data)
        except Exception:
            pass

class SSEHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/events':
            self.send_response(200)
            self.send_header('Content-Type', 'text/event-stream')
            self.send_header('Cache-Control', 'no-cache')
            self.send_header('Connection', 'keep-alive')
            self.end_headers()
            q = queue.Queue()
            CLIENTS.append(q)
            try:
                # send initial full payload
                with metrics_lock:
                    init = json.dumps(metrics)
                self.wfile.write(f"data: {init}\n\n".encode('utf-8'))
                self.wfile.flush()
                while True:
                    data = q.get()
                    self.wfile.write(f"data: {data}\n\n".encode('utf-8'))
                    self.wfile.flush()
            except (BrokenPipeError, ConnectionResetError):
                pass
            finally:
                try:
                    CLIENTS.remove(q)
                except ValueError:
                    pass
            return

        # serve files from web/ by delegating to SimpleHTTPRequestHandler
        if self.path == '/' or self.path.startswith('/index.html'):
            self.path = '/index.html'
        return http.server.SimpleHTTPRequestHandler.do_GET(self)

class ThreadingHTTPServer(socketserver.ThreadingMixIn, http.server.HTTPServer):
    daemon_threads = True

def run_http_server(port):
    handler = SSEHandler
    # serve files relative to project's web/ directory
    import os
    webdir = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'web')
    os.chdir(webdir)
    # Bind explicitly to IPv4 localhost to avoid IPv6-only bind issues
    server = ThreadingHTTPServer(('127.0.0.1', port), handler)
    print(f"Serving web UI on http://127.0.0.1:{port}")
    server.serve_forever()


def start_heartbeat(interval=1.0):
    """Periodically broadcast current metrics to connected SSE clients."""
    def hb():
        while True:
            time.sleep(interval)
            with metrics_lock:
                pass
            # broadcast current snapshot (no-op if metrics empty)
            try:
                broadcast_update()
            except Exception:
                pass
    t = threading.Thread(target=hb, daemon=True)
    t.start()

def run_simulator_and_parse(servers, duration, cmd=None, logfile=None):
    """Run the simulator (or tail a file) and parse lines incrementally."""
    if logfile:
        f = open(logfile, 'r', encoding='utf-8', errors='ignore')
        # seek to start
        for line in f:
            process_line(line)
        # tail remaining lines
        while True:
            where = f.tell()
            line = f.readline()
            if not line:
                time.sleep(0.1); continue
            process_line(line)
    else:
        # run the simulator and pipe inputs
        # ensure we're running from project root so ./lb_sim is found
        project_root = os.path.dirname(os.path.dirname(__file__))
        shell_cmd = f"printf \"{servers}\\n{duration}\\n\" | ./lb_sim"
        print(f"[live_server] Running shell command in {project_root}: {shell_cmd}")
        p = subprocess.Popen(shell_cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, cwd=project_root)
        if not p.stdout:
            print("[live_server] Simulator produced no stdout")
            return
        for line in p.stdout:
            print(f"[sim] {line.rstrip()}")
            process_line(line)

def process_line(line: str):
    line = line.strip() + '\n'
    m = T_RE.search(line)
    updated = False
    if m:
        t = int(m.group(1)); servers_n = int(m.group(2)); queued = int(m.group(3))
        with metrics_lock:
            metrics['metrics'].append({'t': t, 'servers': servers_n, 'queued': queued})
        updated = True
    m = ASSIGN_RE.search(line)
    if m:
        with metrics_lock:
            metrics['assigns'].append({'server': int(m.group(1)), 'req': int(m.group(2)), 'ip_in': m.group(3), 'ip_out': m.group(4), 'cycles': int(m.group(5))})
        updated = True
    m = SCALE_RE.search(line)
    if m:
        with metrics_lock:
            metrics['scales'].append({'action': m.group(1), 'server': int(m.group(2))})
        updated = True
    if updated:
        broadcast_update()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--servers', type=int, default=3)
    parser.add_argument('--duration', type=int, default=100)
    parser.add_argument('--port', type=int, default=8000)
    parser.add_argument('--keep-alive', action='store_true', help='Keep HTTP server alive after simulation completes')
    parser.add_argument('--logfile', type=str, default='')
    args = parser.parse_args()

    # start HTTP server thread (daemon so we can optionally keep main thread blocked)
    t = threading.Thread(target=run_http_server, args=(args.port,), daemon=True)
    t.start()

    # run simulator or tail logfile
    run_simulator_and_parse(args.servers, args.duration, logfile=(args.logfile or None))

    # If requested, keep the main thread alive so the HTTP server remains available
    if args.keep_alive:
        print(f"Simulation finished — keeping web UI available on http://127.0.0.1:{args.port} (Ctrl-C to stop)")
        try:
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            print("Shutting down (KeyboardInterrupt)")

if __name__ == '__main__':
    main()
