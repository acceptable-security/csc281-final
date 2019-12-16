from flask import Flask, escape, request, url_for, redirect, render_template
import random
import subprocess
import threading
import time

app = Flask(__name__)

globalPorts = list(range(10000, 50000))
timeout = 5 * 60
waiting_partners = []

def spawn_servers_thread(ports, portsAvailable):
    global globalPorts
    if len(globalPorts) == 0:
        raise Exception("Out of available ports")

    # Get ports
    websocketA = str(globalPorts.pop())
    websocketB = str(globalPorts.pop())
    alob = str(globalPorts.pop())

    # Generate fifos
    suffix = str(random.randrange(100000, 9999999))

    fifoInA = "/tmp/tmpinA-" + suffix + ".fifo"
    fifoOutA = "/tmp/tmpoutA-" + suffix + ".fifo"

    fifoInB = "/tmp/tmpinB-" + suffix + ".fifo"
    fifoOutB = "/tmp/tmpoutB-" + suffix + ".fifo"

    # Spawn gwsocket servers
    wsA = subprocess.Popen([
        'gwsocket', '-p', websocketA,
                    "--pipein=" + fifoInA,
                    "--pipeout=" + fifoOutA,
                    "--strict"
    ])

    wsB = subprocess.Popen([
        'gwsocket', '-p', websocketB,
                    "--pipein=" + fifoInB,
                    "--pipeout=" + fifoOutB,
                    "--strict"
    ])

    # Spawn alob servers
    abA = subprocess.Popen([
        '../emp-sh2pc/build/bin/alob', fifoOutA, fifoInA, alob
    ])

    abB = subprocess.Popen([
        '../emp-sh2pc/build/bin/alob', fifoOutB, fifoInB, alob
    ])

    print("Done spawning, setting...")
    ports += [ websocketA, websocketB ]
    portsAvailable.set()
    print("set")

    # Wait on the first socket, then kill ther est
    wsA.wait(timeout=timeout)
    wsB.kill()
    abA.kill()
    abB.kill()

    # Return ports
    globalPorts += [ websocketA, websocketB, alob ]

def spawn_servers():
    ports = []
    portsAvailable = threading.Event()

    tr = threading.Thread(target=spawn_servers_thread, args=(ports, portsAvailable))
    tr.start()
    portsAvailable.wait()

    return ports[0], ports[1]

@app.route('/find_partner')
def find_partner():
    global waiting_partners
    # This is disgusting, sorry.

    if len(waiting_partners) == 0:
        port = []

        waiting_partners.append(lambda _found: port.append(_found))

        count = 0

        while len(port) == 0:
            if count > 60:
                return "Failed"

            count += 1
            time.sleep(1)

        return redirect(url_for('main_page', party='A', port=port[0]))
    else:
        partner, waiting_partners = waiting_partners[0], waiting_partners[1:]

        portA, portB = spawn_servers()
        partner(portB)

        return redirect(url_for('main_page', party='B', port=portA))

@app.route('/main/<port>/<party>')
def main_page(port, party):
    return render_template("main.html", party=party, port=port)

@app.route('/match')
def match():
    return render_template("match.html")

@app.route('/no_match')
def no_match():
    return render_template("nomatch.html")


@app.route('/')
def hello():
    return render_template("index.html")

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8000, debug=True, threaded=True)