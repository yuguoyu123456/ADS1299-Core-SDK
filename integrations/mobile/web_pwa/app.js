(() => {
  const MAX_SAMPLES = 500;
  const CHANNELS = 8;

  const canvas = document.getElementById('plot');
  const ctx = canvas.getContext('2d');
  const wsUrl = document.getElementById('wsUrl');
  const connectBtn = document.getElementById('connectBtn');
  const clearBtn = document.getElementById('clearBtn');
  const stateEl = document.getElementById('state');
  const sequenceEl = document.getElementById('sequence');
  const timestampEl = document.getElementById('timestamp');
  const framesEl = document.getElementById('frames');
  const gapsEl = document.getElementById('gaps');
  const statusEl = document.getElementById('status');

  const data = Array.from({ length: CHANNELS }, () => []);
  let socket = null;
  let receivedFrames = 0;
  let sequenceGaps = 0;
  let lastSequence = null;
  let needsDraw = false;

  function resetData() {
    data.forEach(ch => ch.splice(0, ch.length));
    receivedFrames = 0;
    sequenceGaps = 0;
    lastSequence = null;
    framesEl.textContent = '0';
    gapsEl.textContent = '0';
    sequenceEl.textContent = '-';
    timestampEl.textContent = '-';
    statusEl.textContent = '-';
    needsDraw = true;
  }

  function pushFrame(frame) {
    if (!Array.isArray(frame.channels) || frame.channels.length !== CHANNELS) return;

    if (lastSequence !== null) {
      const expected = (lastSequence + 1) >>> 0;
      if ((frame.sequence >>> 0) !== expected) sequenceGaps += 1;
    }
    lastSequence = frame.sequence >>> 0;
    receivedFrames += 1;

    for (let ch = 0; ch < CHANNELS; ch += 1) {
      data[ch].push(Number(frame.channels[ch]) || 0);
      if (data[ch].length > MAX_SAMPLES) data[ch].shift();
    }

    sequenceEl.textContent = String(frame.sequence);
    timestampEl.textContent = String(frame.timestamp_us);
    framesEl.textContent = String(receivedFrames);
    gapsEl.textContent = String(sequenceGaps);
    statusEl.textContent = frame.status_hex || '-';
    needsDraw = true;
  }

  function drawGrid(width, height) {
    ctx.clearRect(0, 0, width, height);
    ctx.save();
    ctx.strokeStyle = 'rgba(127,127,127,0.22)';
    ctx.lineWidth = 1;

    for (let ch = 0; ch <= CHANNELS; ch += 1) {
      const y = (ch / CHANNELS) * height;
      ctx.beginPath();
      ctx.moveTo(0, y);
      ctx.lineTo(width, y);
      ctx.stroke();
    }

    for (let i = 0; i <= 10; i += 1) {
      const x = (i / 10) * width;
      ctx.beginPath();
      ctx.moveTo(x, 0);
      ctx.lineTo(x, height);
      ctx.stroke();
    }
    ctx.restore();
  }

  function draw() {
    if (!needsDraw) return;
    needsDraw = false;

    const width = canvas.width;
    const height = canvas.height;
    drawGrid(width, height);

    let absMax = 1;
    for (const ch of data) {
      for (const v of ch) absMax = Math.max(absMax, Math.abs(v));
    }

    const laneHeight = height / CHANNELS;
    const scale = laneHeight * 0.40 / absMax;

    for (let ch = 0; ch < CHANNELS; ch += 1) {
      const samples = data[ch];
      if (samples.length < 2) continue;

      const centerY = laneHeight * (ch + 0.5);
      ctx.beginPath();
      ctx.lineWidth = 1.4;
      ctx.strokeStyle = `hsl(${(ch * 43) % 360} 70% 50%)`;

      samples.forEach((value, i) => {
        const x = (i / Math.max(1, MAX_SAMPLES - 1)) * width;
        const y = centerY - value * scale;
        if (i === 0) ctx.moveTo(x, y);
        else ctx.lineTo(x, y);
      });
      ctx.stroke();

      ctx.fillStyle = 'rgba(127,127,127,0.9)';
      ctx.font = '13px system-ui';
      ctx.fillText(`CH${ch + 1}`, 8, centerY - laneHeight * 0.28);
    }
  }

  function renderLoop() {
    draw();
    requestAnimationFrame(renderLoop);
  }

  function disconnect() {
    if (socket) {
      socket.onopen = null;
      socket.onclose = null;
      socket.onerror = null;
      socket.onmessage = null;
      socket.close();
      socket = null;
    }
    stateEl.textContent = 'Disconnected';
    connectBtn.textContent = 'Connect';
  }

  function connect() {
    disconnect();
    stateEl.textContent = 'Connecting...';
    socket = new WebSocket(wsUrl.value.trim());

    socket.onopen = () => {
      stateEl.textContent = 'Connected';
      connectBtn.textContent = 'Disconnect';
    };

    socket.onclose = () => {
      stateEl.textContent = 'Disconnected';
      connectBtn.textContent = 'Connect';
      socket = null;
    };

    socket.onerror = () => {
      stateEl.textContent = 'Connection error';
    };

    socket.onmessage = event => {
      try {
        const message = JSON.parse(event.data);
        if (message.type === 'eeg') pushFrame(message);
      } catch (err) {
        console.warn('Ignoring invalid gateway message', err);
      }
    };
  }

  connectBtn.addEventListener('click', () => {
    if (socket && socket.readyState <= WebSocket.OPEN) disconnect();
    else connect();
  });

  clearBtn.addEventListener('click', resetData);
  resetData();
  renderLoop();
})();
