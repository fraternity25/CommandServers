from flask import Flask, request, jsonify, render_template
import subprocess

app = Flask(__name__)

# Route to serve HTML page
@app.route('/')
def index():
    return render_template('index.html')

# Endpoint to receive messages (POST)
@app.route('/receive_message', methods=['POST'])
def receive_message():
    try:
        data = request.get_json()
        message = data.get('message')

        if message is None:
            return jsonify({'error': 'Message field is required'}), 400

        # Store the message for the CommandClient to retrieve later
        with open('messages.txt', 'a') as f:
            f.write(message + '\n')
        
        return jsonify({'message': 'Message received'}), 200

    except Exception as e:
        return jsonify({'error': str(e)}), 500

# Endpoint to retrieve messages (GET)
@app.route('/receive_message', methods=['GET'])
def get_message():
    try:
        with open('messages.txt', 'r') as f:
            messages = f.readlines()
        if messages:
            message = messages.pop(0).strip()
            with open('messages.txt', 'w') as f:
                f.writelines(messages)
            return jsonify({'message': message}), 200
        else:
            return jsonify({'message': ''}), 200

    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    subprocess.run(["cmd.exe", "/c", "g++ colorline.cpp -o colorline.exe"], shell=True)
    subprocess.run(["cmd.exe", "/c", "g++ CustomCmd.cpp -o CustomCmd.exe"], shell=True)
    '""';subprocess.run(["cmd.exe", "/c", "javac java\\CommandServer.java"], shell=True)
    '""';subprocess.run(["cmd.exe", "/c", "javac java\\CommandClient.java"], shell=True)
    '"#"';subprocess.Popen(["cmd.exe", "/c", "java java\\CommandServer"], shell=True)
    '"#"';subprocess.Popen(["cmd.exe", "/c", "java java\\CommandClient use_api"], shell=True)
    app.run(host='172.29.80.1', port=80)  
