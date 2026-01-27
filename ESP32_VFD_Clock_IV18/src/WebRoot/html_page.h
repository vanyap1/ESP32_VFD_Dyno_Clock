
#ifndef INDEX_HTML_H
#define INDEX_HTML_H
const char charGen[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Dino CLOCK DIY Project</title>
  <meta charset="UTF-8">
  <link rel="stylesheet" type="text/css" href="/styles.css">
</head>
<body>
  <div class="main-container">
    <div class="header">
      <h1>Dino CLOCK DIY Project</h1>
      <div class="version-info">Version: 2.0 | ESP32 Controller | Dino CLOCK</div>
    </div>

    <div class="device-info-bar" id="deviceInfoBar">
      <div class="device-info-item">
        <span class="device-info-label">IP:</span>
        <span class="device-info-value" id="deviceIP">---.---.---.---</span>
      </div>
      <span class="device-info-separator">|</span>
      <div class="device-info-item">
        <span class="device-info-label">Ports:</span>
        <span class="device-info-value" id="devicePorts">---, ---</span>
      </div>
      <span class="device-info-separator">|</span>
      <div class="device-info-item">
        <span class="device-info-label">Software:</span>
        <span class="device-info-value" id="swVersion">v-.-.--</span>
      </div>
      <span class="device-info-separator">|</span>
      <div class="device-info-item">
        <span class="device-info-label">Uptime:</span>
        <span class="device-info-value" id="uptime">--:--:--</span>
      </div>
      <span class="device-info-separator">|</span>
      <div class="device-info-item">
        <span class="device-info-label">Status:</span>
        <span class="device-info-value" id="deviceStatus">UNKNOWN</span>
      </div>
    </div>

    <div class="top-control-bar">
      <div class="nav-menu">
        <a href="/" class="nav-button">MainPage</a>
        <a href="/segConstr" class="nav-button">Seg Contructor</a>

      </div>
      
      <div class="system-controls">
        <!--
        <div class="remote-indicator" id="remoteStatus">
          <div>🔴</div>
          <div>REMOTE: <span id="remoteText">INACTIVE</span></div>
        </div>
        
        <button class="emergency-btn" onclick="sendCommand('EMERGENCY_STOP')">EMERGENCY</button>
        -->
        <div class="auto-update-control">
          <label for="autoUpdate">Auto Update:</label>
          <input type="checkbox" id="autoUpdate" onchange="toggleAutoUpdate()" checked>
        </div>
      </div>
    </div>

    <div class="terminal-section">
      <div class="label" style="display: flex; align-items: center; justify-content: space-between; flex-wrap: wrap; gap: 15px;">
        <span>Segment Constructor</span>
        <div style="display: flex; align-items: center; gap: 10px;">
          <label style="color: #09ff00; font-size: 16px;">Display Type:</label>
          <select id="displayType" style="background: #2a2a2a; color: #09ff00; border: 2px solid #09ff00; padding: 3px 10px; font-size: 14px; width: 180px;">
            <option value="7seg">7-Segment (8 bits)</option>
            <option value="16seg">16-Segment (24 bits)</option>
          </select>
        </div>
      </div>
      
      <!-- Horizontal layout for SVG and ASCII table -->
      <div style="display: flex; gap: 30px; align-items: flex-start; justify-content: center; flex-wrap: wrap;">
        
        <!-- SVG Display Container -->
        <div style="flex: 0 0 auto;">
          <!-- 7-Segment Display -->
          <svg id="svg7seg" width="400px" height="280px" viewBox="0 0 1216 1119" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
              <title>xvisd</title>
              <g id="xvisd" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
                  
                  <polygon id="0" onclick="s(this,1)" fill="#3D3D3D" points="845.481445 88.6474609 880.673828 123.547852 845.481445 158.540039 646.317383 158.540039 610.5 123.59375 646.317383 88.6474609"></polygon>
                  <text x="745.586274" y="123.59375" text-anchor="middle" fill="green" font-size="40">A</text>
                  
                  <polygon id="1" onclick="s(this,6)" fill="#3D3D3D" points="883.121094 125.294922 849.228516 159.235352 849.228516 524.225586 883.121094 559.959961 918.039062 524.225586 918.039062 159.235352"></polygon>
                  <text x="883.121094" y="342.627529" text-anchor="middle" fill="green" font-size="40">B</text>
                  
                  <polygon id="2" onclick="s(this,3)" fill="#3D3D3D" points="883.121094 560.294922 849.228516 594.235352 849.228516 959.225586 883.121094 994.959961 918.039062 959.225586 918.039062 594.235352"></polygon>
                  <text x="883.121094" y="777.127954" text-anchor="middle" fill="green" font-size="40">C</text>
                  
                  <polygon id="3" onclick="s(this,7)" fill="#3D3D3D" points="845.481445 962.647461 880.673828 997.547852 845.481445 1032.54004 646.317383 1032.54004 610.5 997.59375 646.317383 962.647461"></polygon>
                  <text x="745.586274" y="997.59375" text-anchor="middle" fill="green" font-size="40">D</text>
                  
                  <polygon id="4" onclick="s(this,8)" fill="#3D3D3D" points="608.121094 560.294922 574.228516 594.235352 574.228516 959.225586 608.121094 994.959961 643.039062 959.225586 643.039062 594.235352"></polygon>
                  <text x="608.121094" y="777.127954" text-anchor="middle" fill="green" font-size="40">E</text>
                  
                  <polygon id="5" onclick="s(this,5)" fill="#3D3D3D" points="608.121094 125.294922 574.228516 159.235352 574.228516 524.225586 608.121094 559.959961 643.039062 524.225586 643.039062 159.235352"></polygon>
                  <text x="608.121094" y="342.627529" text-anchor="middle" fill="green" font-size="40">F</text>

                  <polygon id="6" onclick="s(this,4)" fill="#3D3D3D" points="845.481445 524.647461 880.673828 559.547852 845.481445 594.540039 646.317383 594.540039 610.5 559.59375 646.317383 524.647461"></polygon>
                  <text x="745.586274" y="559.59375" text-anchor="middle" fill="green" font-size="40">G</text>
                  
                  <ellipse id="7" onclick="s(this,2)" cx="979" cy="976" fill="#3f3f3f" id="svg_19" rx="31" ry="30" stroke="#000000" stroke-width="5"/>
                  <text x="979" y="976" text-anchor="middle" fill="green" font-size="40">db</text>
              </g>
          </svg>          
          <!-- 16-Segment Display -->
          <svg id="svg16seg" width="400px" height="280px" viewBox="0 0 1216 1119" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" style="display: none;">
              <title>16seg</title>
              <g id="xvisd" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
                
                
                <polygon id="0" onclick="s(this,1)" fill="#3D3D3D" points="572.481445 88.6474609 604.673828 123.547852 569.481445 158.540039 376.317383 158.540039 340.5 123.59375 376.317383 88.6474609"></polygon>
                <text x="491.051063" y="123.59375" text-anchor="middle" fill="green" font-size="40">A1</text>
                
                <polygon id="1" onclick="s(this,2)" fill="#3D3D3D" points="845.481445 88.6474609 880.673828 123.547852 845.481445 158.540039 646.317383 158.540039 610.5 123.59375 646.317383 88.6474609"></polygon>
                <text x="745.586274" y="123.59375" text-anchor="middle" fill="green" font-size="40">A2</text>

                <polygon id="2" onclick="s(this,3)" fill="#3D3D3D" points="883.121094 125.294922 849.228516 159.235352 849.228516 524.225586 883.121094 559.959961 918.039062 524.225586 918.039062 159.235352"></polygon>
                <text x="883.121094" y="342.627529" text-anchor="middle" fill="green" font-size="40">B</text>

                <polygon id="3" onclick="s(this,4)" fill="#3D3D3D" points="883.121094 560.294922 849.228516 594.235352 849.228516 959.225586 883.121094 994.959961 918.039062 959.225586 918.039062 594.235352"></polygon>
                <text x="883.121094" y="777.127954" text-anchor="middle" fill="green" font-size="40">C</text>

                <polygon id="4" onclick="s(this,5)" fill="#3D3D3D" points="845.481445 962.647461 880.673828 997.547852 845.481445 1032.54004 646.317383 1032.54004 610.5 997.59375 646.317383 962.647461"></polygon>
                <text x="745.586274" y="997.59375" text-anchor="middle" fill="green" font-size="40">D1</text>

                <polygon id="5" onclick="s(this,6)" fill="#3D3D3D" points="572.481445 962.647461 604.673828 997.547852 569.481445 1032.54004 376.317383 1032.54004 340.5 997.59375 376.317383 962.647461"></polygon>
                <text x="491.051063" y="997.59375" text-anchor="middle" fill="green" font-size="40">D2</text>

                <polygon id="6" onclick="s(this,7)" fill="#3D3D3D" points="337.121094 560.294922 303.228516 594.235352 303.228516 959.225586 337.121094 994.959961 372.039062 959.225586 372.039062 594.235352"></polygon>
                <text x="337.121094" y="777.127954" text-anchor="middle" fill="green" font-size="40">E</text>
                
                <polygon id="7" onclick="s(this,8)" fill="#3D3D3D" points="337.121094 125.294922 303.228516 159.235352 303.228516 524.225586 337.121094 559.959961 372.039062 524.225586 372.039062 159.235352"></polygon>
                <text x="337.121094" y="342.627529" text-anchor="middle" fill="green" font-size="40">F</text>
                
                <polygon id="8" onclick="s(this,9)" fill="#3D3D3D" points="572.481445 524.647461 604.673828 559.547852 569.481445 594.540039 376.317383 594.540039 340.5 559.59375 376.317383 524.647461"></polygon>
                <text x="491.051063" y="559.59375" text-anchor="middle" fill="green" font-size="40">G1</text>

                <polygon id="9" onclick="s(this,10)" fill="#3D3D3D" points="845.481445 524.647461 880.673828 559.547852 845.481445 594.540039 646.317383 594.540039 610.5 559.59375 646.317383 524.647461"></polygon>
                <text x="745.586274" y="559.59375" text-anchor="middle" fill="green" font-size="40">G2</text>

                <polygon id="10" onclick="s(this,11)" fill="#3D3D3D" points="376.382812 166.453125 376.382812 257.988281 550.488281 520.607422 566.919922 520.607422 566.919922 428.955078 392.929688 166.453125"></polygon>
                <text x="471.651367" y="343.530273" text-anchor="middle" fill="green" font-size="40">H</text>

                <polygon id="11" onclick="s(this,12)" fill="#3D3D3D" points="608.121094 125.294922 574.228516 159.235352 574.228516 524.225586 608.121094 559.959961 643.039062 524.225586 643.039062 159.235352"></polygon>
                <text x="608.121094" y="342.627529" text-anchor="middle" fill="green" font-size="40">I</text>

                <polygon id="12" onclick="s(this,13)" fill="#3D3D3D" transform="translate(745.651367, 343.530273) scale(-1, 1) translate(-745.651367, -343.530273)" points="650.382812 166.453125 650.382812 257.988281 824.488281 520.607422 840.919922 520.607422 840.919922 428.955078 666.929688 166.453125"></polygon>
                <text x="745.651367" y="343.530273" text-anchor="middle" fill="green" font-size="40">J</text>
                
                <polygon id="13" onclick="s(this,14)" fill="#3D3D3D" transform="translate(745.651367, 781.530273) scale(-1, -1) translate(-745.651367, -781.530273)" points="650.382812 604.453125 650.382812 695.988281 824.488281 958.607422 840.919922 958.607422 840.919922 866.955078 666.929688 604.453125"></polygon>
                <text x="745.651367" y="781.530273" text-anchor="middle" fill="green" font-size="40">K</text>
                
                <polygon id="14" onclick="s(this,15)" fill="#3D3D3D" points="608.121094 560.294922 574.228516 594.235352 574.228516 959.225586 608.121094 994.959961 643.039062 959.225586 643.039062 594.235352"></polygon>
                <text x="608.121094" y="777.127954" text-anchor="middle" fill="green" font-size="40">L</text>

                <polygon id="15" onclick="s(this,16)" fill="#3D3D3D" transform="translate(471.651367, 781.530273) scale(1, -1) translate(-471.651367, -781.530273)" points="376.382812 604.453125 376.382812 695.988281 550.488281 958.607422 566.919922 958.607422 566.919922 866.955078 392.929688 604.453125"></polygon>
                <text x="471.651367" y="781.530273" text-anchor="middle" fill="green" font-size="40">M</text>
                
                
                <ellipse id="16" onclick="s(this,17)" cx="979" cy="976" fill="#3f3f3f" id="svg_19" rx="31" ry="30" stroke="#000000" stroke-width="5"/>
                <text x="979" y="976" text-anchor="middle" fill="green" font-size="40">db</text>
                <rect id="17" onclick="s(this,18)" fill="#3f3f3f" height="60" id="svg_20" stroke="#000000" stroke-width="5" width="42" x="962" y="1021"/>
                <text x="983" y="1056" text-anchor="middle" fill="green" font-size="40">C1</text>
                <rect id="18" onclick="s(this,19)" fill="#3f3f3f" height="91" id="svg_21" stroke="#000000" stroke-width="5" width="51" x="224" y="113"/>
                <text x="249" y="158" text-anchor="middle" fill="green" font-size="40">C2</text>
              </g>
          </svg>        </div>
        
        <!-- ASCII Table -->
        <div style="flex: 1 1 auto; max-width: 800px;">
          <div style="color: #09ff00; font-size: 18px; margin-bottom: 10px; text-align: center;">ASCII Character Table (click to edit)</div>
          <div id="asciiTable" style="display: grid; grid-template-columns: repeat(16, 1fr); gap: 3px;">
            <!-- ASCII table will be generated by JavaScript -->
          </div>
        </div>
        
      </div>
    </div>
    
    <!-- Bit mapping section -->
    <div class="terminal-section">
      <div style="color: #09ff00; font-size: 20px; margin-bottom: 15px; display: flex; align-items: center; flex-wrap: wrap; gap: 15px;">
        <span>Editing Character: <span id="currentChar" style="color: white; font-size: 24px;">-</span> 
        (ASCII: <span id="currentAscii" style="color: white;">-</span>)</span>
        <span style="display: flex; align-items: center; gap: 5px;">
          <span>Position:</span>
          <select id="charPos" style="background: #2a2a2a; color: #09ff00; border: 2px solid #09ff00; padding: 3px 8px; font-size: 16px; width: 60px;">
            <option value="0">0</option>
            <option value="1">1</option>
            <option value="2">2</option>
            <option value="3">3</option>
            <option value="4">4</option>
            <option value="5">5</option>
            <option value="6">6</option>
            <option value="7">7</option>
            <option value="8">8</option>
            <option value="9">9</option>
            <option value="10">10</option>
            <option value="11">11</option>
            <option value="12">12</option>
            <option value="13">13</option>
            <option value="14">14</option>
            <option value="15">15</option>
            <option value="16">16</option>
          </select>
        </span>
      </div>
      
      <div id="bitContainer" style="display: flex; flex-wrap: wrap; gap: 5px; justify-content: center;">
        <!-- Bits will be generated by JavaScript -->
      </div>
      <div style="margin-top: 20px; text-align: center;">
        <span style="color: #09ff00; font-size: 24px; font-family: monospace;">HEX: <span id="hexOutput">0x000000</span></span>
      </div>
      <div style="margin-top: 15px; text-align: center;">
        <button id="sendCharset" style="background: #09ff00; color: black; border: none; padding: 10px 20px; font-size: 16px; cursor: pointer; border-radius: 5px; font-weight: bold;">
          Send CHARSET Command
        </button>
        <div id="commandStatus" style="margin-top: 10px; color: #09ff00; font-size: 14px;"></div>
      </div>
    </div>

    <div class="terminal-section">
      <div class="label">SCPI COMMAND TERMINAL</div>
      <div class="input-group">
        <label for="scpiInput">Enter SCPI Command:</label>
        <input type="text" id="scpiInput" placeholder="e.g., INFO?" onkeypress="handleEnter(event)">
      </div>
      <div class="terminal-buttons">
        <button class="psu-button" id="sendScpiCommand">Send Command</button>
        <button class="psu-button" id="clearScpiTerminal">Clear Terminal</button>
      </div>
      
      <div class="terminal-output" id="terminalOutput">SCPI Terminal Ready

</div>
    </div>
  </div>
  </div>
  
  <style>
        body {background-color: black;}
        .result {
            color: #09ff00;
            font-size: 25px;
            font-family: Arial, Helvetica, sans-serif;
        }
    </style>
<script>
        arr = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0] // 24 bits for 16-segment
        bitArray = new Array(24).fill(0); // 24 bits for 3 bytes
        segmentMapping = {}; // Maps bit number to segment (a-g, dp)
        currentEditingChar = null; // Currently editing character
        charPatterns = {}; // Store custom patterns for each character
        currentDisplayType = '7seg'; // Current display type

        // Get max segments based on display type
        function getMaxSegments() {
            return currentDisplayType === '7seg' ? 8 : 24;
        }

        // Initialize ASCII table
        function initAsciiTable() {
            const table = document.getElementById('asciiTable');
            table.innerHTML = '';
            
            // Create cells for ASCII 32-126 (printable characters)
            for (let i = 32; i <= 126; i++) {
                const cell = document.createElement('div');
                const char = String.fromCharCode(i);
                cell.style.cssText = `
                    width: 35px;
                    height: 35px;
                    background: #2a2a2a;
                    border: 2px solid #09ff00;
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    color: #09ff00;
                    font-family: monospace;
                    font-size: 14px;
                    cursor: pointer;
                    user-select: none;
                    transition: all 0.2s;
                `;
                cell.textContent = i === 32 ? '␣' : char; // Show special symbol for space
                cell.title = `ASCII ${i}: ${char}`;
                cell.dataset.ascii = i;
                cell.onclick = () => selectCharForEditing(i);
                table.appendChild(cell);
            }
        }

        // Select character for editing
        function selectCharForEditing(asciiCode) {
            currentEditingChar = asciiCode;
            const char = String.fromCharCode(asciiCode);
            
            // Update UI
            document.getElementById('currentChar').textContent = asciiCode === 32 ? '(Space)' : char;
            document.getElementById('currentAscii').textContent = asciiCode;
            
            // Highlight selected character in table
            document.querySelectorAll('#asciiTable > div').forEach(cell => {
                if (parseInt(cell.dataset.ascii) === asciiCode) {
                    cell.style.background = '#09ff00';
                    cell.style.color = 'black';
                } else {
                    cell.style.background = '#2a2a2a';
                    cell.style.color = '#09ff00';
                }
            });
            
            // Load character pattern
            loadCharacterPattern(asciiCode);
        }

        // Initialize bit display
        function initBitDisplay() {
            const container = document.getElementById('bitContainer');
            container.innerHTML = '';
            
            // Create 24 bits (from bit 23 to bit 0)
            for (let i = 23; i >= 0; i--) {
                const bitWrapper = document.createElement('div');
                bitWrapper.style.cssText = 'display: flex; flex-direction: column; align-items: center;';
                
                // Dropdown for segment selection
                const select = document.createElement('select');
                select.id = `seg-${i}`;
                select.style.cssText = 'margin-bottom: 5px; background: #2a2a2a; color: #09ff00; border: 1px solid #09ff00; padding: 2px; font-size: 11px;';
                
                // Different options based on display type
                if (currentDisplayType === '7seg') {
                    select.innerHTML = `
                        <option value="">--</option>
                        <option value="a">A</option>
                        <option value="b">B</option>
                        <option value="c">C</option>
                        <option value="d">D</option>
                        <option value="e">E</option>
                        <option value="f">F</option>
                        <option value="g">G</option>
                        <option value="dp">DP</option>
                    `;
                } else {
                    // 16-segment options
                    select.innerHTML = `
                        <option value="">--</option>
                        <option value="a1">A1</option>
                        <option value="a2">A2</option>
                        <option value="b">B</option>
                        <option value="c">C</option>
                        <option value="d1">D1</option>
                        <option value="d2">D2</option>
                        <option value="e">E</option>
                        <option value="f">F</option>
                        <option value="g1">G1</option>
                        <option value="g2">G2</option>
                        <option value="h">H</option>
                        <option value="i">I</option>
                        <option value="j">J</option>
                        <option value="k">K</option>
                        <option value="l">L</option>
                        <option value="m">M</option>
                        <option value="dp">DP</option>
                        <option value="c1">C1</option>
                        <option value="c2">C2</option>
                    `;
                }
                select.onchange = () => updateSegmentMapping(i, select.value);
                
                // Bit square
                const bitSquare = document.createElement('div');
                bitSquare.id = `bit-${i}`;
                bitSquare.style.cssText = `
                    width: 40px; 
                    height: 40px; 
                    background: #3D3D3D; 
                    border: 2px solid #09ff00; 
                    display: flex; 
                    align-items: center; 
                    justify-content: center;
                    color: #09ff00;
                    font-family: monospace;
                    font-size: 12px;
                    cursor: pointer;
                    user-select: none;
                `;
                bitSquare.textContent = i;
                bitSquare.onclick = () => toggleBit(i);
                
                bitWrapper.appendChild(select);
                bitWrapper.appendChild(bitSquare);
                container.appendChild(bitWrapper);
            }
            updateHexOutput();
        }

        function toggleBit(bitNum) {
            bitArray[bitNum] = bitArray[bitNum] ? 0 : 1;
            const bitSquare = document.getElementById(`bit-${bitNum}`);
            bitSquare.style.background = bitArray[bitNum] ? 'white' : '#3D3D3D';
            bitSquare.style.color = bitArray[bitNum] ? 'black' : '#09ff00';
            updateHexOutput();
        }

        function updateSegmentMapping(bitNum, segment) {
            // Remove old mapping if this segment was mapped to another bit
            if (segment) {
                for (let key in segmentMapping) {
                    if (key === segment && segmentMapping[key] !== bitNum) {
                        // Segment was already mapped to another bit - remove it
                        const oldBitNum = segmentMapping[key];
                        const oldSelect = document.getElementById(`seg-${oldBitNum}`);
                        if (oldSelect) {
                            oldSelect.value = '';
                        }
                        delete segmentMapping[key];
                    }
                }
            }
            
            // Remove old mapping if this bit was mapped to another segment
            for (let key in segmentMapping) {
                if (segmentMapping[key] === bitNum) {
                    delete segmentMapping[key];
                }
            }
            
            // Add new mapping
            if (segment) {
                segmentMapping[segment] = bitNum;
            }
        }

        function updateHexOutput() {
            // Convert bit array to hex (bit 23 is MSB, bit 0 is LSB)
            let value = 0;
            for (let i = 0; i < 24; i++) {
                if (bitArray[i]) {
                    value |= (1 << i);
                }
            }
            const hexString = '0x' + value.toString(16).toUpperCase().padStart(6, '0');
            document.getElementById('hexOutput').textContent = hexString;
            
            // Save pattern for current character
            if (currentEditingChar !== null) {
                charPatterns[currentEditingChar] = value;
                // Auto-send command when pattern changes with CHARTEST prefix
                autoSendCharsetCommand('CHARTEST');
            }
        }
        
        // Auto-send charset command on each change
        function autoSendCharsetCommand(cmdPrefix) {
            if (currentEditingChar === null) return;
            
            const charPos = document.getElementById('charPos').value;
            const asciiHex = currentEditingChar.toString(16).toUpperCase().padStart(2, '0');
            const pattern = charPatterns[currentEditingChar] || 0;
            const patternHex = pattern.toString(16).toUpperCase().padStart(6, '0');
            const command = `${cmdPrefix},${charPos},${asciiHex},${patternHex}`;
            
            // Use sendCommand from commonRest.js
            if (typeof sendCommand === 'function') {
                sendCommand(command);
            }
        }

        // Update segment display based on mapping
        function updateSegmentFromDisplay() {
            // Reset all bits
            bitArray.fill(0);
            
            // Different segment names based on display type
            const segmentNames = currentDisplayType === '7seg' 
                ? ['a', 'dp', 'c', 'g', 'f', 'b', 'd', 'e']
                : ['a1', 'a2', 'b', 'c', 'd1', 'd2', 'e', 'f', 'g1', 'g2', 'h', 'i', 'j', 'k', 'l', 'm', 'dp', 'c1', 'c2'];
            
            // Set bits based on segment mapping and arr state
            for (let i = 0; i < arr.length; i++) {
                if (arr[i]) {
                    const segmentName = segmentNames[i];
                    const bitNum = segmentMapping[segmentName];
                    if (bitNum !== undefined) {
                        bitArray[bitNum] = 1;
                    }
                }
            }
            
            // Update visual display
            for (let i = 0; i < 24; i++) {
                const bitSquare = document.getElementById(`bit-${i}`);
                if (bitSquare) {
                    bitSquare.style.background = bitArray[i] ? 'white' : '#3D3D3D';
                    bitSquare.style.color = bitArray[i] ? 'black' : '#09ff00';
                }
            }
            
            updateHexOutput();
        }

        function gethex() {
            hex = "0x"+parseInt(arr.join(""), 2).toString(16).toUpperCase().padStart(2, '0');
            
            // Update old display elements if they exist
            if (document.getElementById("display")) {
                document.getElementById("display").innerHTML=hex;
            }
            if (document.getElementById("dec")) {
                document.getElementById("dec").innerHTML=parseInt(arr.join(""), 2).toString(10);
            }
            if (document.getElementById("bin")) {
                document.getElementById("bin").innerHTML=arr.join("").match(/.{1,4}/g).join(" ");
            }
            if (document.getElementById("ASbin")) {
                document.getElementById("ASbin").innerHTML = "{0b"+arr.slice(0, 8).join("")+ "},";
            }
            if (document.getElementById("AShex")) {
                document.getElementById("AShex").innerHTML = "{0x"+parseInt(arr.join("").slice(0, 8), 2).toString(16) + "},";
            }
            
            // Update bit mapping display when segment changes
            updateSegmentFromDisplay();
        }

        function s(el,id) {
            arr[id-1] = + !arr[id-1]
            el.setAttribute("fill",arr[id-1] == 1 ? "white":"#3D3D3D")
            gethex()
        }

        // Initialize on page load
        window.addEventListener('load', function() {
            initAsciiTable();
            initBitDisplay();
            
            // Add send charset command handler
            document.getElementById('sendCharset').addEventListener('click', sendCharsetCommand);
            
            // Add SCPI terminal handlers
            document.getElementById('sendScpiCommand').addEventListener('click', sendScpiCommand);
            document.getElementById('clearScpiTerminal').addEventListener('click', clearScpiTerminal);
            
            // Add display type change handler
            document.getElementById('displayType').addEventListener('change', function() {
                currentDisplayType = this.value;
                switchDisplayType();
            });
        });
        
        // Switch between display types
        function switchDisplayType() {
            const svg7seg = document.getElementById('svg7seg');
            const svg16seg = document.getElementById('svg16seg');
            
            if (currentDisplayType === '7seg') {
                svg7seg.style.display = 'block';
                svg16seg.style.display = 'none';
                arr = [0,0,0,0,0,0,0,0];
            } else {
                svg7seg.style.display = 'none';
                svg16seg.style.display = 'block';
                arr = new Array(24).fill(0);
            }
            
            // Clear all segments
            clearAllSegments();
            
            // Regenerate bit display with new segment options
            initBitDisplay();
            
            // Reset current character pattern
            if (currentEditingChar !== null) {
                loadCharacterPattern(currentEditingChar);
            }
        }
        
        // Clear all segments visually
        function clearAllSegments() {
            const maxSegs = getMaxSegments();
            // Clear arr array
            for (let i = 0; i < maxSegs; i++) {
                arr[i] = 0;
            }
            // Clear all SVG elements (7-seg has 8 elements, 16-seg has 19 elements)
            const svgContainer = currentDisplayType === '7seg' 
                ? document.getElementById('svg7seg')
                : document.getElementById('svg16seg');
            const numSvgElements = currentDisplayType === '7seg' ? 8 : 19;
            for (let i = 0; i < numSvgElements; i++) {
                const polygon = svgContainer.getElementById(String(i));
                if (polygon) {
                    polygon.setAttribute("fill", "#3D3D3D");
                }
            }
        }
        async function sendCharsetCommand() {
            if (currentEditingChar === null) {
                document.getElementById('commandStatus').textContent = 'Please select a character first!';
                document.getElementById('commandStatus').style.color = '#ff0000';
                return;
            }
            
            const charPos = document.getElementById('charPos').value;
            const asciiHex = currentEditingChar.toString(16).toUpperCase().padStart(2, '0');
            const pattern = charPatterns[currentEditingChar] || 0;
            const patternHex = pattern.toString(16).toUpperCase().padStart(6, '0');
            const command = `CHARSET,${charPos},${asciiHex},${patternHex}`;
            
            try {
                // Send command and get response
                const response = await fetch(`/cmd=${command}`);
                const result = await response.text();
                
                // Show in status
                document.getElementById('commandStatus').textContent = `✓ ${command} → ${result}`;
                document.getElementById('commandStatus').style.color = '#09ff00';
                
                // Also show in terminal
                const terminal = document.getElementById('terminalOutput');
                if (terminal) {
                    terminal.innerHTML += `<span style="color: #09ff00;">&gt; ${command}</span>\n${result}\n\n`;
                    terminal.scrollTop = terminal.scrollHeight;
                }
            } catch (error) {
                document.getElementById('commandStatus').textContent = `✗ Error: ${error.message}`;
                document.getElementById('commandStatus').style.color = '#ff0000';
            }
        }

        // Load character pattern based on ASCII code
        function loadCharacterPattern(charCode) {
            const maxSegs = getMaxSegments();
            
            // Simply load stored pattern if exists, or clear display
            if (charPatterns[charCode] !== undefined) {
                const pattern = charPatterns[charCode];
                // Convert pattern to segment display
                updateSegmentsFromPattern(pattern);
            } else {
                // Clear arr array
                for (let i = 0; i < maxSegs; i++) {
                    arr[i] = 0;
                }
                // Clear all SVG elements
                const svgContainer = currentDisplayType === '7seg' 
                    ? document.getElementById('svg7seg')
                    : document.getElementById('svg16seg');
                const numSvgElements = currentDisplayType === '7seg' ? 8 : 19;
                for (let i = 0; i < numSvgElements; i++) {
                    const polygon = svgContainer.getElementById(String(i));
                    if (polygon) {
                        polygon.setAttribute("fill", "#3D3D3D");
                    }
                }
                // Clear bit array
                bitArray.fill(0);
                for (let i = 0; i < 24; i++) {
                    const bitSquare = document.getElementById(`bit-${i}`);
                    if (bitSquare) {
                        bitSquare.style.background = '#3D3D3D';
                        bitSquare.style.color = '#09ff00';
                    }
                }
                updateHexOutput();
            }
        }
        
        
        function updateSegmentsFromPattern(pattern) {
            const maxSegs = getMaxSegments();
            
            for (let i = 0; i < maxSegs; i++) {
                arr[i] = 0;
            }
            
            // Clear bit array first
            bitArray.fill(0);
            
            // Different segment names based on display type
            const segmentNames = currentDisplayType === '7seg' 
                ? ['a', 'dp', 'c', 'g', 'f', 'b', 'd', 'e']
                : ['a1', 'a2', 'b', 'c', 'd1', 'd2', 'e', 'f', 'g1', 'g2', 'h', 'i', 'j', 'k', 'l', 'm', 'dp', 'c1', 'c2'];
            
            // Check each bit in pattern and activate corresponding segment
            for (let bitNum = 0; bitNum < 24; bitNum++) {
                if ((pattern >> bitNum) & 1) {
                    bitArray[bitNum] = 1;
                    // Find which segment this bit is mapped to
                    for (let seg in segmentMapping) {
                        if (segmentMapping[seg] === bitNum) {
                            const arrIndex = segmentNames.indexOf(seg);
                            if (arrIndex >= 0) {
                                arr[arrIndex] = 1;
                            }
                        }
                    }
                }
            }
            
            // Update segment visual display
            const svgContainer = currentDisplayType === '7seg' 
                ? document.getElementById('svg7seg')
                : document.getElementById('svg16seg');
            const numSvgElements = currentDisplayType === '7seg' ? 8 : 19;
            for (let i = 0; i < numSvgElements; i++) {
                const polygon = svgContainer.getElementById(String(i));
                if (polygon) {
                    polygon.setAttribute("fill", arr[i] ? "white" : "#3D3D3D");
                }
            }
            
            // Update bit array visual display
            for (let i = 0; i < 24; i++) {
                const bitSquare = document.getElementById(`bit-${i}`);
                if (bitSquare) {
                    bitSquare.style.background = bitArray[i] ? 'white' : '#3D3D3D';
                    bitSquare.style.color = bitArray[i] ? 'black' : '#09ff00';
                }
            }
            
            updateHexOutput();
        }
        
        // Send SCPI command from terminal
        async function sendScpiCommand() {
            const input = document.getElementById('scpiInput');
            const command = input.value.trim();
            
            if (!command) {
                return;
            }
            
            try {
                const response = await fetch(`/cmd=${command}`);
                const result = await response.text();
                
                const terminal = document.getElementById('terminalOutput');
                terminal.innerHTML += `<span style="color: #09ff00;">&gt; ${command}</span>\n${result}\n\n`;
                terminal.scrollTop = terminal.scrollHeight;
                
                input.value = '';
            } catch (error) {
                const terminal = document.getElementById('terminalOutput');
                terminal.innerHTML += `<span style="color: #ff0000;">&gt; ${command}</span>\n<span style="color: #ff0000;">Error: ${error.message}</span>\n\n`;
                terminal.scrollTop = terminal.scrollHeight;
            }
        }
        
        // Clear SCPI terminal
        function clearScpiTerminal() {
            document.getElementById('terminalOutput').innerHTML = 'SCPI Terminal Ready\n\n';
        }
        
        // Handle Enter key in SCPI input
        function handleEnter(event) {
            if (event.key === 'Enter') {
                sendScpiCommand();
            }
        }

    </script>

<script src="/commonRest.js"></script>
  
</body>
</html>
  )rawliteral";

const char apConfig[] = R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WiFi Configuration</title>
    <style>
      body {
        background-color: #121212; /* Темний фон */
        color: #e0e0e0; /* Світлий текст */
        font-family: Arial, sans-serif;
        text-align: center;
      }
      h1 {
        color: #4caf50; /* Зелений заголовок */
      }
      form {
        max-width: 400px;
        margin: auto;
        padding: 20px;
        background-color: #1e1e1e; /* Темно-сірий фон форми */
        border: 2px solid #4caf50; /* Зелена рамка */
        border-radius: 10px;
        box-shadow: 0 0 15px rgba(0, 255, 0, 0.2); /* Зелена тінь */
      }
      select, input[type='text'], input[type='submit'] {
        width: calc(100% - 22px);
        padding: 10px;
        margin: 8px 0;
        border: 1px solid #4caf50; /* Зелена рамка */
        border-radius: 5px;
        background-color: #2c2c2c; /* Темний фон полів */
        color: #e0e0e0; /* Світлий текст */
      }
      input[type='submit'] {
        background-color: #2196f3; /* Синя кнопка */
        color: white;
        cursor: pointer;
        transition: background-color 0.3s ease;
      }
      input[type='submit']:hover {
        background-color: #1976d2; /* Темно-синя кнопка при наведенні */
      }
    </style>
  </head>
  <body>
    <h1>WiFi Configuration</h1>
    <form action="/submit" method="get">
      <label for="ssid">Select SSID:</label><br>
      <select id="ssid" name="ssid">
        %OPTIONS%
      </select><br>
      <label for="pass">Password:</label><br>
      <input type="text" id="pass" name="pass" placeholder="Enter Password"><br>
      <input type="submit" value="Save">
    </form>
  </body>
  </html>
  )rawliteral";

const char index_html[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Settings Page</title>
  <style>
    body {
      background-color: #121212; /* Темний фон */
      color: #e0e0e0; /* Світлий текст */
      font-family: Arial, sans-serif;
      text-align: center;
    }
    h1 {
      color: #4caf50; /* Зелений заголовок */
      text-align: center;
    }
    form {
      max-width: 400px;
      margin: auto;
      padding: 20px;
      background-color: #1e1e1e; /* Темно-сірий фон форми */
      border: 2px solid #4caf50; /* Зелена рамка */
      border-radius: 10px;
      box-shadow: 0 0 15px rgba(0, 255, 0, 0.2); /* Зелена тінь */
    }
    input[type='text'], input[type='datetime-local'], input[type='color'], select, input[type='range'] {
      width: calc(100% - 22px);
      padding: 10px;
      margin: 8px 0;
      border: 1px solid #4caf50; /* Зелена рамка */
      border-radius: 5px;
      background-color: #2c2c2c; /* Темний фон полів */
      color: #e0e0e0; /* Світлий текст */
    }
    input[type='checkbox'] {
      margin: 10px 0;
    }
    input[type='submit'] {
      background-color: #2196f3; /* Синя кнопка */
      color: white;
      padding: 10px 15px;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      transition: background-color 0.3s ease;
    }
    input[type='submit']:hover {
      background-color: #1976d2; /* Темно-синя кнопка при наведенні */
    }
    #ambiLightColor {
      height: 50px;
    }
    hr {
      margin: 20px 0;
      border: 1px solid #4caf50; /* Зелена лінія */
    }
    .range-container {
      position: relative;
      width: 100%;
    }
    .range-label {
      position: absolute;
      top: -30px;
      left: 0;
      width: 100%;
      text-align: center;
      color: #4caf50; /* Зелений текст */
    }
  </style>
</head>
<body>
  <h1>VFD Dyno Clock WiFi*256x64</h1>
  <form id="configForm">
    <label for="ntpServer">Вибір сервера реального часу:</label>
    <select id="ntpServer" name="ntpServer">
      <option value="0">pool.ntp.org</option>
      <option value="1">time.google.com</option>
      <option value="2">time.windows.com</option>
    </select><br>

    <label for="timezone">Вибір часового поясу:</label>
    <select id="timezone" name="timezone">
      <option value="-12">UTC -12:00</option>
      <option value="-11">UTC -11:00</option>
      <option value="-10">UTC -10:00</option>
      <option value="-9">UTC -9:00</option>
      <option value="-8">UTC -8:00</option>
      <option value="-7">UTC -7:00</option>
      <option value="-6">UTC -6:00</option>
      <option value="-5">UTC -5:00</option>
      <option value="-4">UTC -4:00</option>
      <option value="-3">UTC -3:00</option>
      <option value="-2">UTC -2:00</option>
      <option value="-1">UTC -1:00</option>
      <option value="0">UTC 0:00</option>
      <option value="1">UTC +1:00</option>
      <option value="2">UTC +2:00</option>
      <option value="3">UTC +3:00</option>
      <option value="4">UTC +4:00</option>
      <option value="5">UTC +5:00</option>
      <option value="6">UTC +6:00</option>
      <option value="7">UTC +7:00</option>
      <option value="8">UTC +8:00</option>
      <option value="9">UTC +9:00</option>
      <option value="10">UTC +10:00</option>
      <option value="11">UTC +11:00</option>
      <option value="12">UTC +12:00</option>
    </select><br>
    
    <hr>
    <label for="demoType">Вибір фонової анімації:</label>
    <select id="demoType" name="demoConf">
      <option value="0">Disabled</option>
      <option value="1">Space Trasher</option>
      <option value="2">Asteroid Rain</option>
      <option value="3">Google Dyno</option>
    </select><br>

    <input type="checkbox" id="manualTime" name="manualTime">
    <label for="manualTime">Не синхронізувати час:</label><br><br>
    
    <label for="datetime">Встановити дату та час:</label>
    <input type="datetime-local" id="datetime" name="datetime"><br>
    
    <label for="ambiLightColor">Фонова підсвітка (Колір):</label>
    <input type="color" id="ambiLightColor" name="ambiLightColor"><br>
    
    <hr>
    <br>
    <div class="range-container">
      <div class="range-label" id="brightnessLabel">Яскравість: 50</div>
      <label for="brightness">Фонова підсвітка (яскравість):</label>
      <input type="range" id="brightness" name="brightness" min="0" max="255" value="50"><br>
    </div>
    
    <label for="ambiLight">Включити підсвітку:</label>
    <input type="checkbox" id="ambiLight" name="ambiLight"><br>
  </form>

  <footer>
    <p>Проєкт доступний на <a href="https://github.com/vanyap1/ESP32_VFD_Dyno_Clock" target="_blank">GitHub</a>.</p>
  </footer>
  
  <script>
    const configForm = document.getElementById('configForm');
    
    const updateBrightnessLabel = () => {
      const brightnessInput = document.getElementById('brightness');
      const brightnessLabel = document.getElementById('brightnessLabel');
      brightnessLabel.textContent = `Яскравість: ${brightnessInput.value}`;
    };

    document.getElementById('brightness').addEventListener('input', updateBrightnessLabel);

    const loadSettings = () => {
      fetch('/settings')
        .then(response => response.json())
        .then(settings => {
          Object.keys(settings).forEach(key => {
            const element = document.querySelector(`[name=${key}]`);
            if (element) {
              if (element.type === 'checkbox') {
                element.checked = settings[key] === 'on';
              } else if (element.type === 'range') {
                element.value = settings[key];
                updateBrightnessLabel();
              } else {
                element.value = settings[key];
              }
            }
          });
        })
        .catch(error => {
          console.error('Error:', error);
        });
    };

    window.onload = loadSettings;

    const handleChange = (event) => {
      const formData = new FormData(configForm);
      const fieldName = event.target.name;
      const fieldValue = formData.get(fieldName);

      const data = { [fieldName]: fieldValue };

      fetch('/submit', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
      })
      .then(response => response.text())
      .then(data => {
        console.log(`Settings saved for ${fieldName}:`, data);
      })
      .catch(error => {
        console.error('Error:', error);
      });
    };

    configForm.addEventListener('change', handleChange);
    configForm.addEventListener('submit', function(event) {
      event.preventDefault();
    });
  </script>
</body>
</html>

)rawliteral";

#endif