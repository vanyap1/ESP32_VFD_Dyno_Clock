
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
        <a href="/chargen" class="nav-button active">Seg Contructor</a>
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
          Save Character to Device
        </button>
        <button id="saveSegmentsMap" style="background: #0099ff; color: black; border: none; padding: 10px 20px; font-size: 16px; cursor: pointer; border-radius: 5px; font-weight: bold; margin-left: 10px;">
          Save SegmentsMap
        </button>
        <button id="dumpCharTab" style="background: #ff9900; color: black; border: none; padding: 10px 20px; font-size: 16px; cursor: pointer; border-radius: 5px; font-weight: bold; margin-left: 10px;">
          DumpCharTab
        </button>
        <button id="loadCharTab" style="background: #9900ff; color: black; border: none; padding: 10px 20px; font-size: 16px; cursor: pointer; border-radius: 5px; font-weight: bold; margin-left: 10px;">
          LoadCharTab
        </button>
        <input type="file" id="fileInput" accept=".bin" style="display: none;">
        <div id="commandStatus" style="margin-top: 10px; color: #09ff00; font-size: 14px;"></div>
      </div>
    </div>

    <div class="terminal-section">
      <div class="label" style="color: #ff6600;">⚠️ FIRMWARE UPDATE</div>
      <div style="text-align: center; padding: 20px;">
        <p style="color: #ff9900; margin-bottom: 15px;">Upload new firmware (.bin file) to update ESP32 controller</p>
        <button id="loadFirmware" style="background: #ff3300; color: white; border: none; padding: 12px 30px; font-size: 18px; cursor: pointer; border-radius: 5px; font-weight: bold;">
          📤 Load Firmware
        </button>
        <input type="file" id="firmwareInput" accept=".bin" style="display: none;">
        <div id="firmwareStatus" style="margin-top: 15px; color: #09ff00; font-size: 14px;"></div>
        <div style="margin-top: 10px; color: #888; font-size: 12px;">⚠️ Device will restart after successful update</div>
      </div>
    </div>

    <div class="info-section">
      <div class="label">Commands examples</div>
      <table class="command-table">
        <tr>
          <th>Command</th>
          <th>Description</th>
          <th>Example</th>
          <th>Response</th>
        </tr>
        <tr>
          <td>CHARSET,... <span class="copy-icon" onclick="copyToInput('CHARSET,0,41,0F3F00')" title="Copy to input">📋</span></td>
          <td>Save character to device</td>
          <td>CHARSET,0,41,0F3F00</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>CHARTEST,... <span class="copy-icon" onclick="copyToInput('CHARTEST,0,41,0F3F00')" title="Copy to input">📋</span></td>
          <td>Test character (real-time)</td>
          <td>CHARTEST,0,41,0F3F00</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>SEG=... <span class="copy-icon" onclick="copyToInput('SEG=a,b,c,d,e,f,g,dp')" title="Copy to input">📋</span></td>
          <td>Save segment mapping</td>
          <td>SEG=a,b,c,d,e,f,g,dp</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>SEG? <span class="copy-icon" onclick="copyToInput('SEG?')" title="Copy to input">📋</span></td>
          <td>Get segment mapping</td>
          <td>SEG?</td>
          <td>a,b,c,d,e,f,g,dp...</td>
        </tr>
        <tr>
          <td>DUMP? <span class="copy-icon" onclick="copyToInput('DUMP?')" title="Copy to input">📋</span></td>
          <td>Download character table</td>
          <td>DUMP?</td>
          <td>Binary file download</td>
        </tr>
        <tr>
          <td>LOAD <span class="copy-icon" onclick="copyToInput('LOAD')" title="Copy to input">📋</span></td>
          <td>Upload character table</td>
          <td>LOAD (POST binary)</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>FIRMWARE <span class="copy-icon" onclick="copyToInput('FIRMWARE')" title="Copy to input">📋</span></td>
          <td>Upload firmware (OTA)</td>
          <td>FIRMWARE (POST binary)</td>
          <td>OK, Rebooting...</td>
        </tr>
        <tr>
          <td>TEXT &lt;value&gt; <span class="copy-icon" onclick="copyToInput('TEXT 123456')" title="Copy to input">📋</span></td>
          <td>Show text on display</td>
          <td>TEXT 123456</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>BRIGHT &lt;0-100&gt; <span class="copy-icon" onclick="copyToInput('BRIGHT 80')" title="Copy to input">📋</span></td>
          <td>Set display brightness</td>
          <td>BRIGHT 80</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>INFO? <span class="copy-icon" onclick="copyToInput('INFO?')" title="Copy to input">📋</span></td>
          <td>Get device information</td>
          <td>INFO?</td>
          <td>Device Info String</td>   
        </tr>
        <tr>
          <td>RESTART <span class="copy-icon" onclick="copyToInput('RESTART')" title="Copy to input">📋</span></td>
          <td>Restart the device</td>
          <td>RESTART</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>CLOCK <span class="copy-icon" onclick="copyToInput('CLOCK')" title="Copy to input">📋</span></td>
          <td>Back to clock display</td>
          <td>CLOCK</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>SETTIME &lt;value&gt; <span class="copy-icon" onclick="copyToInput('SETTIME 12:34:56')" title="Copy to input">📋</span></td>
          <td>Set the device time</td>
          <td>SETTIME 12:34:56</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>SETDATE &lt;value&gt; <span class="copy-icon" onclick="copyToInput('SETDATE 2024-06-01')" title="Copy to input">📋</span></td>
          <td>Set the device date</td>
          <td>SETDATE 2024-06-01</td>
          <td>OK</td>
        </tr>

      </table>
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
            
            // Add save segments map handler
            document.getElementById('saveSegmentsMap').addEventListener('click', saveSegmentsMap);
            
            // Add dump and load char table handlers
            document.getElementById('dumpCharTab').addEventListener('click', dumpCharTab);
            document.getElementById('loadCharTab').addEventListener('click', () => {
                document.getElementById('fileInput').click();
            });
            document.getElementById('fileInput').addEventListener('change', loadCharTab);
            
            // Add firmware update handler
            document.getElementById('loadFirmware').addEventListener('click', () => {
                document.getElementById('firmwareInput').click();
            });
            document.getElementById('firmwareInput').addEventListener('change', loadFirmware);
            
            // Add SCPI terminal handlers
            document.getElementById('sendScpiCommand').addEventListener('click', sendScpiCommand);
            document.getElementById('clearScpiTerminal').addEventListener('click', clearScpiTerminal);
            
            // Add display type change handler
            document.getElementById('displayType').addEventListener('change', function() {
                currentDisplayType = this.value;
                switchDisplayType();
            });
            
            // Load segments map from server
            loadSegmentsMap();
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
        // Dump character table from device
        async function dumpCharTab() {
            try {
                const response = await fetch('/cmd=DUMP?');
                
                if (!response.ok) {
                    throw new Error(`HTTP error! status: ${response.status}`);
                }
                
                // Get binary data as blob
                const blob = await response.blob();
                
                // Try to use File System Access API (modern browsers)
                if ('showSaveFilePicker' in window) {
                    try {
                        const defaultFileName = `chartab_${new Date().toISOString().slice(0,19).replace(/[:.]/g, '-')}.bin`;
                        const handle = await window.showSaveFilePicker({
                            suggestedName: defaultFileName,
                            types: [{
                                description: 'Binary Files',
                                accept: {'application/octet-stream': ['.bin']},
                            }],
                        });
                        
                        const writable = await handle.createWritable();
                        await writable.write(blob);
                        await writable.close();
                        
                        document.getElementById('commandStatus').textContent = `✓ Character table saved successfully`;
                        document.getElementById('commandStatus').style.color = '#09ff00';
                    } catch (err) {
                        if (err.name === 'AbortError') {
                            document.getElementById('commandStatus').textContent = 'Save cancelled';
                            document.getElementById('commandStatus').style.color = '#ffaa00';
                            return;
                        }
                        throw err;
                    }
                } else {
                    // Fallback for older browsers
                    const url = window.URL.createObjectURL(blob);
                    const a = document.createElement('a');
                    a.href = url;
                    a.download = `chartab_${new Date().toISOString().slice(0,19).replace(/[:.]/g, '-')}.bin`;
                    document.body.appendChild(a);
                    a.click();
                    window.URL.revokeObjectURL(url);
                    document.body.removeChild(a);
                    
                    document.getElementById('commandStatus').textContent = `✓ Character table dumped successfully`;
                    document.getElementById('commandStatus').style.color = '#09ff00';
                }
                
                const terminal = document.getElementById('terminalOutput');
                if (terminal) {
                    terminal.innerHTML += `<span style="color: #09ff00;">&gt; DUMP?</span>\nCharacter table saved to file\n\n`;
                    terminal.scrollTop = terminal.scrollHeight;
                }
            } catch (error) {
                document.getElementById('commandStatus').textContent = `✗ Error: ${error.message}`;
                document.getElementById('commandStatus').style.color = '#ff0000';
            }
        }
        
        // Load character table to device
        async function loadCharTab(event) {
            const file = event.target.files[0];
            if (!file) {
                return;
            }
            
            try {
                // Read file as array buffer
                const arrayBuffer = await file.arrayBuffer();
                
                // Send binary data to server
                const response = await fetch('/cmd=LOAD', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/octet-stream'
                    },
                    body: arrayBuffer
                });
                
                const result = await response.text();
                
                document.getElementById('commandStatus').textContent = `✓ Character table loaded: ${result}`;
                document.getElementById('commandStatus').style.color = '#09ff00';
                
                const terminal = document.getElementById('terminalOutput');
                if (terminal) {
                    terminal.innerHTML += `<span style="color: #09ff00;">&gt; LOAD (${file.name})</span>\n${result}\n\n`;
                    terminal.scrollTop = terminal.scrollHeight;
                }
                
                // Clear file input
                event.target.value = '';
            } catch (error) {
                document.getElementById('commandStatus').textContent = `✗ Error: ${error.message}`;
                document.getElementById('commandStatus').style.color = '#ff0000';
                
                // Clear file input
                event.target.value = '';
            }
        }
        
        // Load firmware to device (OTA update)
        async function loadFirmware(event) {
            const file = event.target.files[0];
            if (!file) {
                return;
            }
            
            // Check file extension
            if (!file.name.endsWith('.bin')) {
                document.getElementById('firmwareStatus').textContent = '✗ Error: Only .bin files are allowed';
                document.getElementById('firmwareStatus').style.color = '#ff0000';
                event.target.value = '';
                return;
            }
            
            // Show file size
            const fileSizeKB = (file.size / 1024).toFixed(2);
            document.getElementById('firmwareStatus').textContent = `Uploading firmware: ${file.name} (${fileSizeKB} KB)...`;
            document.getElementById('firmwareStatus').style.color = '#ffaa00';
            
            try {
                // Read file as array buffer
                const arrayBuffer = await file.arrayBuffer();
                
                // Send firmware to server
                const response = await fetch('/cmd=FIRMWARE', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/octet-stream',
                        'X-File-Size': file.size.toString()
                    },
                    body: arrayBuffer
                });
                
                const result = await response.text();
                
                document.getElementById('firmwareStatus').textContent = `✓ Firmware uploaded: ${result}. Device will restart...`;
                document.getElementById('firmwareStatus').style.color = '#09ff00';
                
                const terminal = document.getElementById('terminalOutput');
                if (terminal) {
                    terminal.innerHTML += `<span style="color: #ff9900;">&gt; FIRMWARE UPDATE (${file.name})</span>\n${result}\n\n`;
                    terminal.scrollTop = terminal.scrollHeight;
                }
                
                // Clear file input
                event.target.value = '';
            } catch (error) {
                document.getElementById('firmwareStatus').textContent = `✗ Error: ${error.message}`;
                document.getElementById('firmwareStatus').style.color = '#ff0000';
                
                // Clear file input
                event.target.value = '';
            }
        }
        
        // Save segments map to device
        async function saveSegmentsMap() {
            // Collect all select values from bit 23 to bit 0
            const segments = [];
            for (let i = 23; i >= 0; i--) {
                const select = document.getElementById(`seg-${i}`);
                const value = select.value || 'x';
                segments.push(value);
            }
            
            const command = `SEG=${segments.join(',')}`;
            
            // Use sendCommand from commonRest.js if available
            if (typeof sendCommand === 'function') {
                sendCommand(command, (error, result) => {
                    if (error) {
                        document.getElementById('commandStatus').textContent = `✗ Error: ${error.message}`;
                        document.getElementById('commandStatus').style.color = '#ff0000';
                    } else {
                        document.getElementById('commandStatus').textContent = `✓ Segments map saved: ${result}`;
                        document.getElementById('commandStatus').style.color = '#09ff00';
                        
                        const terminal = document.getElementById('terminalOutput');
                        if (terminal) {
                            terminal.innerHTML += `<span style="color: #09ff00;">&gt; ${command}</span>\n${result}\n\n`;
                            terminal.scrollTop = terminal.scrollHeight;
                        }
                    }
                });
            } else {
                // Fallback to direct fetch
                try {
                    const response = await fetch(`/cmd=${command}`);
                    const result = await response.text();
                    
                    document.getElementById('commandStatus').textContent = `✓ Segments map saved: ${result}`;
                    document.getElementById('commandStatus').style.color = '#09ff00';
                    
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
        }
        
        // Load segments map from device
        async function loadSegmentsMap() {
            // Use sendCommand from commonRest.js if available
            if (typeof sendCommand === 'function') {
                sendCommand('SEG?', (error, result) => {
                    if (error) {
                        console.error('Failed to load segments map:', error);
                        return;
                    }
                    
                    // Parse response - expecting format like "a,b,c,--,dp,..."
                    const segments = result.trim().split(',');
                    
                    // Set select values from bit 23 to bit 0
                    for (let i = 23; i >= 0; i--) {
                        const select = document.getElementById(`seg-${i}`);
                        const index = 23 - i;
                        if (select && segments[index]) {
                            const value = segments[index].trim();
                            select.value = value === '--' || value === 'x' ? '' : value;
                            // Update segment mapping
                            updateSegmentMapping(i, select.value);
                        }
                    }
                    
                    console.log('Segments map loaded from device');
                });
            } else {
                // Fallback to direct fetch
                try {
                    const response = await fetch('/cmd=SEG?');
                    const result = await response.text();
                    
                    // Parse response - expecting format like "a,b,c,--,dp,..."
                    const segments = result.trim().split(',');
                    
                    // Set select values from bit 23 to bit 0
                    for (let i = 23; i >= 0; i--) {
                        const select = document.getElementById(`seg-${i}`);
                        const index = 23 - i;
                        if (select && segments[index]) {
                            const value = segments[index].trim();
                            select.value = value === '--' || value === 'x' ? '' : value;
                            // Update segment mapping
                            updateSegmentMapping(i, select.value);
                        }
                    }
                    
                    console.log('Segments map loaded from device');
                } catch (error) {
                    console.error('Failed to load segments map:', error);
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
            
            // Use sendCommand from commonRest.js if available
            if (typeof sendCommand === 'function') {
                sendCommand(command, (error, result) => {
                    if (error) {
                        document.getElementById('commandStatus').textContent = `✗ Error: ${error.message}`;
                        document.getElementById('commandStatus').style.color = '#ff0000';
                    } else {
                        // Show in status
                        document.getElementById('commandStatus').textContent = `✓ ${command} → ${result}`;
                        document.getElementById('commandStatus').style.color = '#09ff00';
                        
                        // Also show in terminal
                        const terminal = document.getElementById('terminalOutput');
                        if (terminal) {
                            terminal.innerHTML += `<span style="color: #09ff00;">&gt; ${command}</span>\n${result}\n\n`;
                            terminal.scrollTop = terminal.scrollHeight;
                        }
                    }
                });
            } else {
                // Fallback to direct fetch
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
            
            const terminal = document.getElementById('terminalOutput');
            
            // Use sendCommand from commonRest.js if available
            if (typeof sendCommand === 'function') {
                if (terminal) {
                    terminal.innerHTML += `<span style="color: #09ff00;">&gt; ${command}</span>\n`;
                }
                
                sendCommand(command, (error, result) => {
                    if (terminal) {
                        if (error) {
                            terminal.innerHTML += `<span style="color: #ff0000;">Error: ${error.message}</span>\n\n`;
                        } else {
                            terminal.innerHTML += `${result}\n\n`;
                        }
                        terminal.scrollTop = terminal.scrollHeight;
                    }
                });
                
                input.value = '';
            } else {
                // Fallback to direct fetch
                try {
                    const response = await fetch(`/cmd=${command}`);
                    const result = await response.text();
                    
                    if (terminal) {
                        terminal.innerHTML += `<span style="color: #09ff00;">&gt; ${command}</span>\n${result}\n\n`;
                        terminal.scrollTop = terminal.scrollHeight;
                    }
                    
                    input.value = '';
                } catch (error) {
                    if (terminal) {
                        terminal.innerHTML += `<span style="color: #ff0000;">&gt; ${command}</span>\n<span style="color: #ff0000;">Error: ${error.message}</span>\n\n`;
                        terminal.scrollTop = terminal.scrollHeight;
                    }
                }
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
        
        // Copy command to SCPI input
        function copyToInput(command) {
            document.getElementById('scpiInput').value = command;
            document.getElementById('scpiInput').focus();
        }

    </script>

<script src="/commonRest.js"></script>
  
</body>
</html>
  )rawliteral";


const char index_html[] = R"rawliteral(
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
        <a href="/" class="nav-button active">MainPage</a>
        <a href="/chargen" class="nav-button">Seg Contructor</a>
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
      <div class="label">System Setup</div>
      <!-- WiFi Settings -->
      <div class="setup-group">
        <h3 class="setup-heading">📶 WiFi Settings</h3>
        <div class="setup-row">
          <label class="setup-label">SSID (Network Name):</label>
          <div class="setup-controls">
            <select id="wifiSsid" class="select-input" onchange="toggleManualSsidInput()" style="flex: 1;">
              <option value="">Loading...</option>
            </select>
            <button class="set-button" onclick="refreshWiFiNetworks()" style="margin-left: 10px;">🔄 Refresh</button>
          </div>
        </div>
        <div class="setup-row" id="manualSsidRow" style="display: none;">
          <label class="setup-label">Manual SSID:</label>
          <div class="setup-controls">
            <input type="text" id="manualSsidInput" class="text-input" placeholder="Enter SSID manually">
          </div>
        </div>
        <div class="setup-row">
          <label class="setup-label">Password:</label>
          <div class="setup-controls">
            <input type="password" id="wifiPassword" class="text-input" placeholder="Enter WiFi password">
          </div>
        </div>
        <div class="setup-row">
          <label class="setup-label"></label>
          <div class="setup-controls">
            <button class="set-button" onclick="saveWiFiAndReboot()">SAVE AND REBOOT</button>
          </div>
        </div>
      </div>

      <!-- Date & Time Settings -->
      <div class="setup-group">
        <h3 class="setup-heading">📅 Date & Time Settings</h3>
        <div class="setup-row">
          <label class="setup-label">Date & Time:</label>
          <div class="setup-controls">
            <input type="datetime-local" id="dateTimePicker" class="datetime-input">
            <button class="set-button" onclick="setDateTime()">OK</button>
          </div>
        </div>
        <div class="setup-row">
          <label class="setup-label"></label>
          <div class="setup-controls" style="font-size: 0.9em; color: #888;">
            ℹ️ Time is stored in RTC (I2C RV8803 if available, otherwise ESP32 built-in). NTP sync updates both.
          </div>
        </div>
        <div class="setup-row">
          <label class="setup-label">NTP Server:</label>
          <div class="setup-controls">
            <select id="ntpServer" class="select-input">
              <option value="pool.ntp.org">pool.ntp.org</option>
              <option value="time.google.com">time.google.com</option>
              <option value="time.cloudflare.com">time.cloudflare.com</option>
              <option value="time.windows.com">time.windows.com</option>
              <option value="time.nist.gov">time.nist.gov</option>
            </select>
            <button class="set-button" onclick="setNtpServer()">SET</button>
          </div>
        </div>
        <div class="setup-row">
          <label class="setup-label">Enable NTP Sync:</label>
          <div class="setup-controls">
            <input type="checkbox" id="ntpEnable" class="checkbox-input" checked onchange="setNtpEnable()">
          </div>
        </div>
        <div class="setup-row">
          <label class="setup-label">Timezone:</label>
          <div class="setup-controls">
            <select id="timezone" class="select-input">
              <option value="-12">UTC-12:00 (Baker Island)</option>
              <option value="-11">UTC-11:00 (American Samoa)</option>
              <option value="-10">UTC-10:00 (Hawaii)</option>
              <option value="-9">UTC-09:00 (Alaska)</option>
              <option value="-8">UTC-08:00 (Los Angeles)</option>
              <option value="-7">UTC-07:00 (Denver)</option>
              <option value="-6">UTC-06:00 (Chicago)</option>
              <option value="-5">UTC-05:00 (New York)</option>
              <option value="-4">UTC-04:00 (Atlantic)</option>
              <option value="-3">UTC-03:00 (Buenos Aires)</option>
              <option value="-2">UTC-02:00 (Mid-Atlantic)</option>
              <option value="-1">UTC-01:00 (Azores)</option>
              <option value="0">UTC+00:00 (London)</option>
              <option value="1">UTC+01:00 (Paris)</option>
              <option value="2">UTC+02:00 (Kyiv)</option>
              <option value="3">UTC+03:00 (Moscow)</option>
              <option value="4">UTC+04:00 (Dubai)</option>
              <option value="5">UTC+05:00 (Pakistan)</option>
              <option value="6">UTC+06:00 (Bangladesh)</option>
              <option value="7">UTC+07:00 (Bangkok)</option>
              <option value="8">UTC+08:00 (Beijing)</option>
              <option value="9">UTC+09:00 (Tokyo)</option>
              <option value="10">UTC+10:00 (Sydney)</option>
              <option value="11">UTC+11:00 (Solomon Islands)</option>
              <option value="12">UTC+12:00 (New Zealand)</option>
            </select>
            <button class="set-button" onclick="setTimezone()">SET</button>
          </div>
        </div>
      </div>

      

      <!-- Display Screens -->
      <div class="setup-group">
        <h3 class="setup-heading">🖥️ Display Screens</h3>
        <table class="sensor-table">
          <tr>
            <td class="sensor-control">
              <label class="setup-label">Screen 1:</label>
              <input type="text" id="format1" class="text-input" placeholder="*HH*:*MM*:*SS*">
              <input type="number" id="format1Time" class="number-input" min="1" max="3600" value="10" placeholder="sec">
              <input type="checkbox" id="format1Enable" class="checkbox-input" checked>
              <span class="checkbox-label">Enable</span>
              <input type="checkbox" id="format1Blink" class="checkbox-input">
              <span class="checkbox-label">Blink Point </span>
              <button class="set-button" onclick="setDisplayFormat(1)">SET</button>
            </td>
            
          </tr>
          <tr>
            <td class="sensor-control">
              <label class="setup-label">Screen 2:</label>
              <input type="text" id="format2" class="text-input" placeholder="*DD*.*MO*.*YYYY*">
              <input type="number" id="format2Time" class="number-input" min="1" max="3600" value="5" placeholder="sec">
              <input type="checkbox" id="format2Enable" class="checkbox-input">
              <span class="checkbox-label">Enable</span>
              <input type="checkbox" id="format2Blink" class="checkbox-input">
              <span class="checkbox-label">Blink Point </span>
              <button class="set-button" onclick="setDisplayFormat(2)">SET</button>
            </td>
           
          </tr>
          <tr>
            <td class="sensor-control">
              <label class="setup-label">Screen 3:</label>
              <input type="text" id="format3" class="text-input" placeholder="*TEMP*°C *PRESS*hPa">
              <input type="number" id="format3Time" class="number-input" min="1" max="3600" value="3" placeholder="sec">
              <input type="checkbox" id="format3Enable" class="checkbox-input">
              <span class="checkbox-label">Enable</span>
              <input type="checkbox" id="format3Blink" class="checkbox-input">
              <span class="checkbox-label">Blink Point </span>
              <button class="set-button" onclick="setDisplayFormat(3)">SET</button>
            </td>
            
          </tr>
          <tr>
            <td class="sensor-description">
              <span class="variable-hint">Variables: *HH* *MM* *SS* *DD* *MO* *YY* *YYYY*<br>*TEMP* *PRESS* *HUM* *WTEMP* *WCOND*<br>*EUR* *USD* *BTC*</span>
            </td>
          </tr>
        </table>
      </div>

      <!-- Blinking Point -->
      <div class="setup-group">
        <h3 class="setup-heading">⚡ Blinking Point</h3>
        <div class="setup-row">
          <label class="setup-label">Segment Mask (HEX):</label>
          <div class="setup-controls">
            <input type="text" id="blinkMask" class="text-input" placeholder="0x000000" value="0x000000" style="width: 150px;">
            <span class="variable-hint">Bitmask for segments to blink</span>
          </div>
        </div>
        <div class="setup-row">
          <label class="setup-label">Blink Position:</label>
          <div class="setup-controls">
            <input type="number" id="blinkPosition" class="number-input" min="0" max="16" value="0" placeholder="0-16">
            <span class="variable-hint">Display digit position (0-16)</span>
            <button class="set-button" onclick="setBlinkChar()">SET</button>
          </div>
        </div>
      </div>

      <!-- Sensor Settings -->
      <div class="setup-group">
        <h3 class="setup-heading">🌡️ Sensor Settings</h3>
        <table class="sensor-table">
          <tr>
            <td class="sensor-control">
              <input type="checkbox" id="pressureSensor" class="checkbox-input" onchange="setSensor('pressure', this.checked)">
              <label class="setup-label">Pressure Sensor:</label>
            </td>
            <td class="sensor-description">
              <span class="variable-hint">Variables: *PRESS*, *HUM*<br>Atmospheric pressure in hPa and humidity in %</span>
            </td>
          </tr>
          <tr>
            <td class="sensor-control">
              <input type="checkbox" id="tempSensor" class="checkbox-input" onchange="setSensor('temperature', this.checked)">
              <label class="setup-label">Temperature Sensor:</label>
            </td>
            <td class="sensor-description">
              <span class="variable-hint">Variables: *TEMP*<br>Temperature in degrees Celsius</span>
            </td>
          </tr>
          <tr>
            <td class="sensor-control">
              <input type="checkbox" id="autoBrightness" class="checkbox-input" onchange="setSensor('autobrightness', this.checked)">
              <label class="setup-label">Auto Brightness:</label>
            </td>
            <td class="sensor-description">
              <span class="variable-hint">Automatically adjust display brightness<br>based on ambient light</span>
            </td>
          </tr>
          <tr>
            <td class="sensor-control">
              <input type="checkbox" id="weatherApi" class="checkbox-input" onchange="setSensor('weatherapi', this.checked)">
              <label class="setup-label">Weather API:</label>
            </td>
            <td class="sensor-description">
              <span class="variable-hint">Variables: *WTEMP*, *WCOND*<br>Weather temperature and conditions<br>from online API</span>
            </td>
          </tr>
          <tr>
            <td class="sensor-control">
              <input type="checkbox" id="currency" class="checkbox-input" onchange="setSensor('currency', this.checked)">
              <label class="setup-label">Currency values:</label>
            </td>
            <td class="sensor-description">
              <span class="variable-hint">Variables: *EUR*, *USD*<br>Exchange rates for EUR and USD<br>in your local currency</span>
            </td>
          </tr>
        </table>

        <div class="setup-row">
          <label class="setup-label">Display Brightness:</label>
          <div class="setup-controls">
            <input type="range" id="displayBrightness" class="slider-input" min="0" max="7" value="7">
            <span id="displayBrightnessValue" class="slider-value">7</span>
          </div>
        </div>
      </div>

      <!-- Ambient Lighting Settings -->
      <div class="setup-group">
        <h3 class="setup-heading">💡 Ambient Lighting Settings</h3>
        <div class="setup-row">
          <label class="setup-label">LED Color:</label>
          <div class="setup-controls">
            <input type="color" id="ledColor" class="color-input" value="#00ff00">
            <button class="set-button" onclick="setLedColor()">SET</button>
          </div>
        </div>
        <div class="setup-row">
          <label class="setup-label">LED Brightness:</label>
          <div class="setup-controls">
            <input type="range" id="ledBrightness" class="slider-input" min="0" max="255" value="128">
            <span id="ledBrightnessValue" class="slider-value">128</span>
          </div>
        </div>
        <div class="setup-row">
          <label class="setup-label">LED Count:</label>
          <div class="setup-controls">
            <input type="number" id="ledCount" class="number-input" min="1" max="300" value="16">
            <button class="set-button" onclick="setLedCount()">SET</button>
          </div>
        </div>
        <div class="setup-row">
          <label class="setup-label">LED Effect:</label>
          <div class="setup-controls">
            <select id="ledEffect" class="select-input">
              <option value="solid">Solid Color</option>
              <option value="rainbow">Rainbow</option>
              <option value="breathe">Breathing</option>
              <option value="pulse">Pulse</option>
              <option value="wave">Wave</option>
              <option value="chase">Chase</option>
              <option value="twinkle">Twinkle</option>
            </select>
            <button class="set-button" onclick="setLedEffect()">SET</button>
          </div>
        </div>
      </div>
    </div>
    
    <div class="info-section">
      <div class="label">Commands examples</div>
      <table class="command-table">
        <tr>
          <th>Command</th>
          <th>Description</th>
          <th>Example</th>
          <th>Response</th>
        </tr>
        <tr>
          <td>GET:SETTINGS? <span class="copy-icon" onclick="copyToInput('GET:SETTINGS?')" title="Copy to input">📋</span></td>
          <td>Get all device settings</td>
          <td>GET:SETTINGS?</td>
          <td>JSON with all settings</td>
        </tr>
        <tr>
          <td>DATETIME,... <span class="copy-icon" onclick="copyToInput('DATETIME,2026,1,31,12,30,0')" title="Copy to input">📋</span></td>
          <td>Set date and time (updates ESP32 RTC and I2C RTC if available)</td>
          <td>DATETIME,2026,1,31,12,30,0</td>
          <td>Time set successfully</td>
        </tr>
        <tr>
          <td>I2C:SCAN <span class="copy-icon" onclick="copyToInput('I2C:SCAN')" title="Copy to input">📋</span></td>
          <td>Scan I2C bus for devices</td>
          <td>I2C:SCAN</td>
          <td>[{"address":"0x40","decimal":64},{"address":"0x32","decimal":50}]</td>
        </tr>
        <tr>
          <td>LED:TEST <span class="copy-icon" onclick="copyToInput('LED:TEST')" title="Copy to input">📋</span></td>
          <td>Test LED strip (RGB cycle)</td>
          <td>LED:TEST</td>
          <td>LED test completed</td>
        </tr>
        <tr>
          <td>HDC2010:TEST <span class="copy-icon" onclick="copyToInput('HDC2010:TEST')" title="Copy to input">📋</span></td>
          <td>Test temperature/humidity sensor</td>
          <td>HDC2010:TEST</td>
          <td>HDC2010 sensor OK\nTemp: 23.5 C\nHum: 45.2 %</td>
        </tr>
        <tr>
          <td>RTC:TEST <span class="copy-icon" onclick="copyToInput('RTC:TEST')" title="Copy to input">📋</span></td>
          <td>Test RTC clock module</td>
          <td>RTC:TEST</td>
          <td>RV8803 RTC OK\nTime: 12:34:56\nDate: 02.03.2026</td>
        </tr>
        <tr>
          <td>RTC:SYNC <span class="copy-icon" onclick="copyToInput('RTC:SYNC')" title="Copy to input">📋</span></td>
          <td>Sync RTC with system time</td>
          <td>RTC:SYNC</td>
          <td>RTC synchronized with system time\nSystem time: 12:34:56 02.03.2026</td>
        </tr>
        <tr>
          <td>NTP:SERVER=&lt;idx&gt; <span class="copy-icon" onclick="copyToInput('NTP:SERVER=0')" title="Copy to input">📋</span></td>
          <td>Set NTP server (0-4)</td>
          <td>NTP:SERVER=0</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>NTP:ENABLE=&lt;0/1&gt; <span class="copy-icon" onclick="copyToInput('NTP:ENABLE=1')" title="Copy to input">📋</span></td>
          <td>Enable/disable NTP sync</td>
          <td>NTP:ENABLE=1</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>TIMEZONE=&lt;offset&gt; <span class="copy-icon" onclick="copyToInput('TIMEZONE=2')" title="Copy to input">📋</span></td>
          <td>Set timezone offset in hours (-12 to +12)</td>
          <td>TIMEZONE=2</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>WIFI:SAVE=... <span class="copy-icon" onclick="copyToInput('WIFI:SAVE=MyNet,pass123,0')" title="Copy to input">📋</span></td>
          <td>Save WiFi credentials</td>
          <td>WIFI:SAVE=MyNet,pass123,0</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>REBOOT <span class="copy-icon" onclick="copyToInput('REBOOT')" title="Copy to input">📋</span></td>
          <td>Reboot device</td>
          <td>REBOOT</td>
          <td>Rebooting...</td>
        </tr>
        <tr>
          <td>DISPLAY:SCREEN1=... <span class="copy-icon" onclick="copyToInput('DISPLAY:SCREEN1=*HH*:*MM*,10,1,0')" title="Copy to input">📋</span></td>
          <td>Set display screen 1</td>
          <td>DISPLAY:SCREEN1=*HH*:*MM*,10,1,0</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>BLINK:POINT=&lt;mask,pos&gt; <span class="copy-icon" onclick="copyToInput('BLINK:POINT=0x000001,1')" title="Copy to input">📋</span></td>
          <td>Set blinking point</td>
          <td>BLINK:POINT=0x000001,1</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>DISPLAY:BRIGHTNESS=&lt;0-255&gt; <span class="copy-icon" onclick="copyToInput('DISPLAY:BRIGHTNESS=128')" title="Copy to input">📋</span></td>
          <td>Set display brightness</td>
          <td>DISPLAY:BRIGHTNESS=128</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>SENSOR:PRESSURE=&lt;0/1&gt; <span class="copy-icon" onclick="copyToInput('SENSOR:PRESSURE=1')" title="Copy to input">📋</span></td>
          <td>Enable/disable pressure sensor</td>
          <td>SENSOR:PRESSURE=1</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>SENSOR:TEMPERATURE=&lt;0/1&gt; <span class="copy-icon" onclick="copyToInput('SENSOR:TEMPERATURE=1')" title="Copy to input">📋</span></td>
          <td>Enable/disable temperature sensor</td>
          <td>SENSOR:TEMPERATURE=1</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>SENSOR:AUTOBRIGHTNESS=&lt;0/1&gt; <span class="copy-icon" onclick="copyToInput('SENSOR:AUTOBRIGHTNESS=1')" title="Copy to input">📋</span></td>
          <td>Enable/disable auto brightness</td>
          <td>SENSOR:AUTOBRIGHTNESS=1</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>SENSOR:WEATHERAPI=&lt;0/1&gt; <span class="copy-icon" onclick="copyToInput('SENSOR:WEATHERAPI=1')" title="Copy to input">📋</span></td>
          <td>Enable/disable weather API</td>
          <td>SENSOR:WEATHERAPI=1</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>SENSOR:CURRENCY=&lt;0/1&gt; <span class="copy-icon" onclick="copyToInput('SENSOR:CURRENCY=1')" title="Copy to input">📋</span></td>
          <td>Enable/disable currency values</td>
          <td>SENSOR:CURRENCY=1</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>LED:COLOR=&lt;R,G,B&gt; <span class="copy-icon" onclick="copyToInput('LED:COLOR=255,0,0')" title="Copy to input">📋</span></td>
          <td>Set LED RGB color</td>
          <td>LED:COLOR=255,0,0</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>LED:BRIGHTNESS=&lt;0-255&gt; <span class="copy-icon" onclick="copyToInput('LED:BRIGHTNESS=128')" title="Copy to input">📋</span></td>
          <td>Set LED brightness</td>
          <td>LED:BRIGHTNESS=128</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>LED:COUNT=&lt;1-300&gt; <span class="copy-icon" onclick="copyToInput('LED:COUNT=16')" title="Copy to input">📋</span></td>
          <td>Set LED count</td>
          <td>LED:COUNT=16</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>LED:EFFECT=&lt;idx&gt; <span class="copy-icon" onclick="copyToInput('LED:EFFECT=0')" title="Copy to input">📋</span></td>
          <td>Set LED effect (0-6)</td>
          <td>LED:EFFECT=0</td>
          <td>OK</td>
        </tr>
        <tr>
          <td>TEXT &lt;value&gt; <span class="copy-icon" onclick="copyToInput('TEXT 123456')" title="Copy to input">📋</span></td>
          <td>Show text on display</td>
          <td>TEXT 123456</td>
          <td>OK</td>
        </tr>
      </table>
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
  
<script>
// Helper function to log to terminal
function logToTerminal(command, result, isError) {
  const terminal = document.getElementById('terminalOutput');
  if (terminal) {
    const color = isError ? '#ff0000' : '#09ff00';
    terminal.innerHTML += `<span style="color: ${color};">&gt; ${command}</span>\n${result}\n\n`;
    terminal.scrollTop = terminal.scrollHeight;
  }
}

// Date & Time functions
function setDateTime() {
  const dt = document.getElementById('dateTimePicker').value;
  if (!dt) {
    logToTerminal('DATETIME', 'Error: Please select date and time', true);
    return;
  }
  const date = new Date(dt);
  const cmd = `DATETIME,${date.getFullYear()},${date.getMonth()+1},${date.getDate()},${date.getHours()},${date.getMinutes()},${date.getSeconds()}`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else logToTerminal(cmd, res, false);
  });
}

function setNtpServer() {
  const serverIndex = document.getElementById('ntpServer').selectedIndex;
  const cmd = `NTP:SERVER=${serverIndex}`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else logToTerminal(cmd, res, false);
  });
}

function setNtpEnable() {
  const enabled = document.getElementById('ntpEnable').checked;
  const cmd = `NTP:ENABLE=${enabled ? 1 : 0}`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else logToTerminal(cmd, res || 'OK', false);
  });
}

function setTimezone() {
  const timezoneValue = document.getElementById('timezone').value;
  const cmd = `TIMEZONE=${timezoneValue}`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else logToTerminal(cmd, res, false);
  });
}

// WiFi functions
function populateWiFiSelect(currentSSID, isOffline) {
  const select = document.getElementById('wifiSsid');
  
  if (isOffline) {
    // Offline mode - scan networks once
    const cmd = 'WIFI:SCAN';
    sendCommand(cmd, (err, res) => {
      if (err) {
        select.innerHTML = '<option value="">Enter SSID manually</option>';
        logToTerminal(cmd, 'Error: ' + err.message, true);
        toggleManualSsidInput(); // Update visibility
        return;
      }
      
      try {
        const networks = JSON.parse(res);
        select.innerHTML = '';
        
        // Add option for manual entry
        const manualOption = document.createElement('option');
        manualOption.value = '';
        manualOption.textContent = '-- Enter SSID manually --';
        select.appendChild(manualOption);
        
        // Add scanned networks
        networks.forEach(network => {
          const option = document.createElement('option');
          option.value = network.ssid;
          option.textContent = `${network.ssid} (${network.rssi} dBm)${network.secure ? ' 🔒' : ''}`;
          select.appendChild(option);
        });
        
        console.log(`Found ${networks.length} networks`);
        toggleManualSsidInput(); // Update visibility after populating
      } catch (e) {
        select.innerHTML = '<option value="">Enter SSID manually</option>';
        logToTerminal(cmd, 'Error parsing networks: ' + e.message, true);
        toggleManualSsidInput(); // Update visibility
      }
    });
  } else {
    // Online mode - show current SSID
    select.innerHTML = '';
    
    const manualOption = document.createElement('option');
    manualOption.value = '';
    manualOption.textContent = '-- Enter SSID manually --';
    select.appendChild(manualOption);
    
    if (currentSSID && currentSSID.trim() !== '') {
      const currentOption = document.createElement('option');
      currentOption.value = currentSSID;
      currentOption.textContent = `${currentSSID} (current)`;
      currentOption.selected = true;
      select.appendChild(currentOption);
    }
    
    toggleManualSsidInput(); // Update visibility after populating
  }
}

// Refresh WiFi networks (force rescan)
function refreshWiFiNetworks() {
  const select = document.getElementById('wifiSsid');
  select.innerHTML = '<option value="">Scanning...</option>';
  
  const cmd = 'WIFI:SCAN?force=1';
  sendCommand(cmd, (err, res) => {
    if (err) {
      select.innerHTML = '<option value="">Enter SSID manually</option>';
      logToTerminal(cmd, 'Error: ' + err.message, true);
      toggleManualSsidInput();
      return;
    }
    
    try {
      const networks = JSON.parse(res);
      select.innerHTML = '';
      
      // Add option for manual entry
      const manualOption = document.createElement('option');
      manualOption.value = '';
      manualOption.textContent = '-- Enter SSID manually --';
      select.appendChild(manualOption);
      
      // Add scanned networks
      networks.forEach(network => {
        const option = document.createElement('option');
        option.value = network.ssid;
        option.textContent = `${network.ssid} (${network.rssi} dBm)${network.secure ? ' 🔒' : ''}`;
        select.appendChild(option);
      });
      
      logToTerminal(cmd, `Found ${networks.length} networks`, false);
      toggleManualSsidInput();
    } catch (e) {
      select.innerHTML = '<option value="">Enter SSID manually</option>';
      logToTerminal(cmd, 'Error parsing networks: ' + e.message, true);
      toggleManualSsidInput();
    }
  });
}

// Toggle manual SSID input visibility
function toggleManualSsidInput() {
  const select = document.getElementById('wifiSsid');
  const manualRow = document.getElementById('manualSsidRow');
  const manualInput = document.getElementById('manualSsidInput');
  
  console.log('toggleManualSsidInput called, select.value:', select.value);
  
  if (select.value === '' || select.value === 'Loading...' || select.value === 'Scanning...') {
    manualRow.style.display = 'flex';
    if (select.value !== 'Loading...' && select.value !== 'Scanning...') {
      manualInput.focus();
    }
  } else {
    manualRow.style.display = 'none';
    manualInput.value = '';
  }
}

function saveWiFiAndReboot() {
  const select = document.getElementById('wifiSsid');
  const manualInput = document.getElementById('manualSsidInput');
  let ssid = select.value;
  
  console.log('saveWiFiAndReboot called');
  console.log('Select value:', ssid);
  console.log('Manual input value:', manualInput.value);
  
  // If manual entry selected, get from manual input
  if (!ssid || ssid === '' || ssid === 'Loading...' || ssid === 'Scanning...') {
    ssid = manualInput.value.trim();
    console.log('Using manual SSID:', ssid);
    if (!ssid) {
      alert('Please enter WiFi SSID');
      logToTerminal('WIFI:SAVE', 'Error: SSID is required', true);
      manualInput.focus();
      return;
    }
  }
  
  const password = document.getElementById('wifiPassword').value;
  
  console.log('Saving WiFi - SSID:', ssid, 'Password length:', password.length);
  
  // Security index always 0 (will be auto-detected by ESP32)
  const cmd = `WIFI:SAVE=${ssid},${password},0`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else {
      logToTerminal(cmd, res + '\\nDevice will restart...', false);
    }
  });
}

// Display Screen functions
function setDisplayFormat(num) {
  const format = document.getElementById(`format${num}`).value;
  const time = document.getElementById(`format${num}Time`).value;
  const enabled = document.getElementById(`format${num}Enable`).checked;
  const blink = document.getElementById(`format${num}Blink`).checked;
  const cmd = `DISPLAY:SCREEN${num}=${format},${time},${enabled ? 1 : 0},${blink ? 1 : 0}`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else logToTerminal(cmd, res, false);
  });
}

// Blinking Point function
function setBlinkChar() {
  const mask = document.getElementById('blinkMask').value;
  const position = document.getElementById('blinkPosition').value;
  const cmd = `BLINK:POINT=${mask},${position}`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else logToTerminal(cmd, res, false);
  });
}

// Sensor functions
function setSensor(type, enabled) {
  const cmd = `SENSOR:${type.toUpperCase()}=${enabled ? 1 : 0}`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else logToTerminal(cmd, res || 'OK', false);
  });
}

function setDisplayBrightness() {
  const brightness = document.getElementById('displayBrightness').value;
  const cmd = `DISPLAY:BRIGHTNESS=${brightness}`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else logToTerminal(cmd, res || 'OK', false);
  });
}

// LED functions
function setLedColor() {
  const color = document.getElementById('ledColor').value;
  // Convert hex to RGB
  const r = parseInt(color.substr(1,2), 16);
  const g = parseInt(color.substr(3,2), 16);
  const b = parseInt(color.substr(5,2), 16);
  const cmd = `LED:COLOR=${r},${g},${b}`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else logToTerminal(cmd, res, false);
  });
}

function setLedBrightness() {
  const brightness = document.getElementById('ledBrightness').value;
  const cmd = `LED:BRIGHTNESS=${brightness}`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else logToTerminal(cmd, res || 'OK', false);
  });
}

function setLedCount() {
  const count = document.getElementById('ledCount').value;
  const cmd = `LED:COUNT=${count}`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else logToTerminal(cmd, res, false);
  });
}

function setLedEffect() {
  const effectIndex = document.getElementById('ledEffect').selectedIndex;
  const cmd = `LED:EFFECT=${effectIndex}`;
  sendCommand(cmd, (err, res) => {
    if (err) logToTerminal(cmd, 'Error: ' + err.message, true);
    else logToTerminal(cmd, res, false);
  });
}

// Load all settings from device
function loadSettings() {
  const cmd = 'GET:SETTINGS?';
  sendCommand(cmd, (err, res) => {
    if (err) {
      logToTerminal(cmd, 'Error: ' + err.message, true);
      return;
    }
    
    try {
      const settings = JSON.parse(res);
      
      // WiFi settings
      if (settings.wifi) {
        // Store current SSID in data attribute for later use
        const ssidElement = document.getElementById('wifiSsid');
        if (ssidElement) {
          ssidElement.setAttribute('data-current-ssid', settings.wifi.ssid || '');
        }
      }
      
      // NTP settings
      if (settings.ntp) {
        document.getElementById('ntpServer').selectedIndex = settings.ntp.server || 0;
        document.getElementById('ntpEnable').checked = settings.ntp.enabled || false;
      }
      
      // Timezone (settings.timezone is hour offset -12..+12)
      if (settings.timezone !== undefined) {
        const select = document.getElementById('timezone');
        // Find option with matching value
        for (let i = 0; i < select.options.length; i++) {
          if (parseInt(select.options[i].value) === settings.timezone) {
            select.selectedIndex = i;
            break;
          }
        }
      } else {
        // Default to UTC+2 (Kyiv) if not set
        const select = document.getElementById('timezone');
        for (let i = 0; i < select.options.length; i++) {
          if (parseInt(select.options[i].value) === 2) {
            select.selectedIndex = i;
            break;
          }
        }
      }
      
      // Display screens
      if (settings.formats && settings.formats.length >= 3) {
        for (let i = 0; i < 3; i++) {
          const format = settings.formats[i];
          document.getElementById(`format${i+1}`).value = format.text || '';
          document.getElementById(`format${i+1}Time`).value = format.time || 10;
          document.getElementById(`format${i+1}Enable`).checked = format.enabled || false;
          document.getElementById(`format${i+1}Blink`).checked = format.blink || false;
        }
      }
      
      // Blinking special char
      if (settings.blink) {
        document.getElementById('blinkMask').value = settings.blink.mask || '0x000000';
        document.getElementById('blinkPosition').value = settings.blink.position || 0;
      }
      
      // Sensors
      if (settings.sensors) {
        document.getElementById('pressureSensor').checked = settings.sensors.pressure || false;
        document.getElementById('tempSensor').checked = settings.sensors.temperature || false;
        document.getElementById('autoBrightness').checked = settings.sensors.autobrightness || false;
        document.getElementById('weatherApi').checked = settings.sensors.weatherapi || false;
        document.getElementById('currency').checked = settings.sensors.currency || false;
      }
      
      // Display brightness
      if (settings.display) {
        const brightness = settings.display.brightness || 128;
        document.getElementById('displayBrightness').value = brightness;
        document.getElementById('displayBrightnessValue').textContent = brightness;
      }
      
      // LED settings
      if (settings.led) {
        // LED Color
        if (settings.led.color) {
          const r = settings.led.color.r.toString(16).padStart(2, '0');
          const g = settings.led.color.g.toString(16).padStart(2, '0');
          const b = settings.led.color.b.toString(16).padStart(2, '0');
          document.getElementById('ledColor').value = `#${r}${g}${b}`;
        }
        
        // LED Brightness
        const ledBrightness = settings.led.brightness || 128;
        document.getElementById('ledBrightness').value = ledBrightness;
        document.getElementById('ledBrightnessValue').textContent = ledBrightness;
        
        // LED Count
        document.getElementById('ledCount').value = settings.led.count || 16;
        
        // LED Effect
        document.getElementById('ledEffect').selectedIndex = settings.led.effect || 0;
      }
      
      logToTerminal(cmd, 'Settings loaded successfully', false);
    } catch (e) {
      logToTerminal(cmd, 'Error parsing settings: ' + e.message, true);
    }
  });
}

// Update slider value displays and auto-send on release
document.addEventListener('DOMContentLoaded', function() {
  const displayBrightness = document.getElementById('displayBrightness');
  const displayBrightnessValue = document.getElementById('displayBrightnessValue');
  if (displayBrightness && displayBrightnessValue) {
    displayBrightness.addEventListener('input', function() {
      displayBrightnessValue.textContent = this.value;
    });
    // Auto-send on release
    displayBrightness.addEventListener('change', function() {
      setDisplayBrightness();
    });
  }
  
  const ledBrightness = document.getElementById('ledBrightness');
  const ledBrightnessValue = document.getElementById('ledBrightnessValue');
  if (ledBrightness && ledBrightnessValue) {
    ledBrightness.addEventListener('input', function() {
      ledBrightnessValue.textContent = this.value;
    });
    // Auto-send on release
    ledBrightness.addEventListener('change', function() {
      setLedBrightness();
    });
  }
  
  // Set current date/time as default
  const now = new Date();
  now.setMinutes(now.getMinutes() - now.getTimezoneOffset());
  const dateTimeInput = document.getElementById('dateTimePicker');
  if (dateTimeInput) {
    dateTimeInput.value = now.toISOString().slice(0,16);
  }
  
  // Check device status and populate WiFi list accordingly
  const cmd = 'GET_DEVICE_INFO';
  sendCommand(cmd, (err, res) => {
    if (!err && res) {
      const parts = res.split(',');
      const status = parts[8]; // Status is 9th field (index 8)
      const isOffline = status && status.trim() === 'OFFLINE';
      
      // Get current SSID from settings (will be loaded by loadSettings)
      setTimeout(() => {
        const currentSSID = document.getElementById('wifiSsid').getAttribute('data-current-ssid') || '';
        populateWiFiSelect(currentSSID, isOffline);
      }, 300);
    } else {
      // Default to offline mode on error
      setTimeout(() => {
        populateWiFiSelect('', true);
      }, 300);
    }
  });
  
  // Load all settings from device
  loadSettings();
});
</script>

<script src="/commonRest.js"></script>
  
</body>
</html>
)rawliteral";

#endif