#include <Arduino.h>

const char commonStyles[] PROGMEM = R"rawliteral(
body { 
  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
  margin: 0; 
  padding: 15px; 
  background: linear-gradient(135deg, #1a1a1a 0%, #2d2d30 100%);
  color: #e0e0e0;
  min-height: 100vh;
  font-size: 14px;
}
.main-container {
  max-width: 1400px;
  margin: 0 auto;
  background: #2d2d30;
  border-radius: 12px;
  padding: 20px;
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
  border: 1px solid #404040;
}
.header {
  text-align: center;
  margin-bottom: 15px;
  border-bottom: 2px solid #00bcd4;
  padding-bottom: 15px;
}
.header h1 {
  color: #00bcd4;
  font-size: 2.2em;
  margin: 0;
  text-shadow: 0 0 10px rgba(0, 188, 212, 0.3);
}
.version-info {
  color: #888;
  font-size: 0.85em;
  margin-top: 5px;
}
.device-info-bar {
  display: flex;
  justify-content: space-around;
  align-items: center;
  background: #252525;
  border-radius: 8px;
  padding: 12px 20px;
  border: 1px solid #404040;
  margin-bottom: 15px;
  font-size: 0.85em;
  flex-wrap: wrap;
  gap: 20px;
}
.device-info-item {
  display: flex;
  align-items: center;
  gap: 8px;
  color: #bbb;
  white-space: nowrap;
}
.device-info-item:last-child {
  margin-bottom: 0;
}
.device-info-label {
  color: #888;
  font-weight: bold;
}
.device-info-value {
  color: #e0e0e0;
  font-weight: bold;
  margin-left: 5px;
}
.device-info-separator {
  color: #555;
  margin: 0 15px;
}
.status-ready {
  color: #4caf50;
}
.status-fault {
  color: #f44336;
}
/* Combined top bar with navigation and system controls */
.top-control-bar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 20px;
  margin-bottom: 20px;
  padding: 15px;
  background: #1e1e1e;
  border-radius: 8px;
  border: 2px solid #404040;
  flex-wrap: wrap;
}
.nav-menu {
  display: flex;
  gap: 15px;
  align-items: center;
}
.system-controls {
  display: flex;
  gap: 15px;
  align-items: center;
  flex-wrap: wrap;
}
.nav-button {
  padding: 8px 16px;
  background: linear-gradient(45deg, #404040, #505050);
  color: #e0e0e0;
  text-decoration: none;
  border-radius: 6px;
  transition: all 0.3s ease;
  border: 1px solid #555;
  font-size: 0.85em;
  white-space: nowrap;
}
.nav-button:hover {
  background: linear-gradient(45deg, #00bcd4, #26c6da);
  color: white;
  box-shadow: 0 4px 15px rgba(0, 188, 212, 0.3);
}
.nav-button.active {
  background: linear-gradient(45deg, #00bcd4, #26c6da);
  color: white;
  box-shadow: 0 4px 15px rgba(0, 188, 212, 0.3);
}
/* Remote indicator and emergency button styling */
.remote-indicator {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 8px 12px;
  border-radius: 5px;
  border: 2px solid;
  font-weight: bold;
  font-size: 0.8em;
  white-space: nowrap;
}
.remote-active {
  background: rgba(244, 67, 54, 0.2);
  border-color: #f44336;
  color: #f44336;
}
.remote-inactive {
  background: rgba(76, 175, 80, 0.2);
  border-color: #4caf50;
  color: #4caf50;
}
.emergency-btn {
  padding: 8px 16px;
  background: linear-gradient(45deg, #ff5722, #ff7043);
  color: white;
  border: none;
  border-radius: 5px;
  font-size: 0.85em;
  font-weight: bold;
  cursor: pointer;
  transition: all 0.3s ease;
  box-shadow: 0 3px 12px rgba(255, 87, 34, 0.3);
  white-space: nowrap;
}
.emergency-btn:hover {
  background: linear-gradient(45deg, #ff7043, #ff5722);
  box-shadow: 0 5px 16px rgba(255, 87, 34, 0.4);
}
.auto-update-control {
  display: flex;
  align-items: center;
  gap: 6px;
  color: #bbb;
  font-size: 0.8em;
  white-space: nowrap;
}
.auto-update-control input[type="checkbox"] {
  transform: scale(1.2);
}

/* Dual Channel Layout - оптимізований */
.channels-container {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 20px;
  margin-bottom: 20px;
}
.channel-section {
  background: #1e1e1e;
  border-radius: 12px;
  padding: 18px;
  border: 3px solid #404040;
  transition: all 0.3s ease;
}
.channel-section.channel-active {
  border-color: #00bcd4;
  box-shadow: 0 0 15px rgba(0, 188, 212, 0.2);
}
.channel-section.channel-inactive {
  background: #2a2a2a;
  border-color: #555;
  opacity: 0.6;
}
.channel-header {
  text-align: center;
  margin-bottom: 15px;
  padding-bottom: 10px;
  border-bottom: 2px solid #404040;
}
.channel-title {
  color: #00bcd4;
  font-size: 1.5em;
  margin: 0;
  text-shadow: 0 0 10px rgba(0, 188, 212, 0.3);
}
.channel-inactive .channel-title {
  color: #666;
}

/* Channel Display - horizontal voltage/current layout */
.channel-display {
  margin-bottom: 15px;
}
.main-values {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 15px;
  margin-bottom: 10px;
}
.value-display {
  text-align: center;
}
.big-value {
  font-size: 1.9em;
  font-weight: bold;
  margin: 3px 0;
  text-shadow: 0 0 15px;
}
.voltage-value { color: #4caf50; text-shadow: 0 0 15px rgba(76, 175, 80, 0.5); }
.current-value { color: #ff9800; text-shadow: 0 0 15px rgba(255, 152, 0, 0.5); }
.unit {
  font-size: 0.45em;
  color: #bbb;
  margin-left: 5px;
}
.label {
  color: #bbb;
  font-size: 0.8em;
  margin-bottom: 3px;
}
.secondary-info {
  background: #252525;
  border-radius: 6px;
  padding: 10px;
  margin-top: 8px;
}
.temp-power {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 10px;
  text-align: center;
}
.small-value {
  font-size: 1.2em;
  font-weight: bold;
  color: #00bcd4;
  margin: 3px 0;
}

/* Channel Status - компактний */
.channel-status {
  display: flex;
  justify-content: space-around;
  margin-bottom: 12px;
}
.status-indicator {
  text-align: center;
  padding: 8px;
  border-radius: 5px;
  background: #2a2a2a;
  min-width: 50px;
  font-size: 0.75em;
}
.status-active {
  background: rgba(76, 175, 80, 0.2);
  border: 2px solid #4caf50;
  color: #4caf50;
  box-shadow: 0 0 8px rgba(76, 175, 80, 0.3);
}
.status-inactive {
  background: rgba(244, 67, 54, 0.2);
  border: 2px solid #f44336;
  color: #f44336;
}
.set-values {
  background: #252525;
  border-radius: 6px;
  padding: 10px;
  text-align: center;
}
.set-value-row {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 10px;
  margin-bottom: 6px;
}
.set-value {
  color: #888;
  font-size: 0.9em;
}

/* Channel Controls - компактні */
.channel-controls {
  margin-top: 12px;
}
.control-inputs {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 10px;
  margin-bottom: 12px;
}
.input-group {
  background: #2a2a2a;
  padding: 10px;
  border-radius: 6px;
  border: 1px solid #555;
}
.input-group label {
  display: block;
  color: #bbb;
  margin-bottom: 4px;
  font-weight: bold;
  font-size: 0.8em;
}
.input-group input {
  width: 100%;
  padding: 6px;
  background: #1a1a1a;
  border: 2px solid #555;
  border-radius: 4px;
  color: #e0e0e0;
  font-size: 0.9em;
  box-sizing: border-box;
  font-family: monospace;
}
.input-group input:focus {
  border-color: #00bcd4;
  outline: none;
  box-shadow: 0 0 5px rgba(0, 188, 212, 0.3);
}
.channel-buttons {
  display: flex;
  justify-content: center;
  gap: 8px;
  flex-wrap: wrap;
}
.psu-button {
  padding: 8px 16px;
  border: none;
  border-radius: 5px;
  font-size: 0.85em;
  font-weight: bold;
  cursor: pointer;
  transition: all 0.3s ease;
  min-width: 80px;
}
.btn-power-on {
  background: linear-gradient(45deg, #4caf50, #66bb6a);
  color: white;
  box-shadow: 0 3px 12px rgba(76, 175, 80, 0.3);
}
.btn-power-on:hover:not(:disabled) {
  background: linear-gradient(45deg, #66bb6a, #4caf50);
  box-shadow: 0 5px 16px rgba(76, 175, 80, 0.4);
}
.btn-power-off {
  background: linear-gradient(45deg, #f44336, #ef5350);
  color: white;
  box-shadow: 0 3px 12px rgba(244, 67, 54, 0.3);
}
.btn-power-off:hover:not(:disabled) {
  background: linear-gradient(45deg, #ef5350, #f44336);
  box-shadow: 0 5px 16px rgba(244, 67, 54, 0.4);
}
.btn-set {
  background: linear-gradient(45deg, #00bcd4, #26c6da);
  color: white;
  box-shadow: 0 3px 12px rgba(0, 188, 212, 0.3);
}
.btn-set:hover:not(:disabled) {
  background: linear-gradient(45deg, #26c6da, #00bcd4);
  box-shadow: 0 5px 16px rgba(0, 188, 212, 0.4);
}
.psu-button:disabled {
  opacity: 0.5;
  cursor: not-allowed;
  box-shadow: none;
}

#response {
  background: #252525;
  border: 1px solid #555;
  border-radius: 5px;
  padding: 12px;
  margin-top: 15px;
  color: #00bcd4;
  font-family: monospace;
  min-height: 40px;
  max-height: 100px;
  overflow-y: auto;
  font-size: 0.85em;
}

/* Responsive для малих екранів */
@media (max-height: 900px) {
  .big-value { font-size: 1.9em; }
  .channel-section { padding: 15px; }
  .main-container { padding: 15px; }
}

@media (max-height: 800px) {
  .big-value { font-size: 1.7em; }
  .channel-title { font-size: 1.3em; }
  .header h1 { font-size: 1.9em; }
}

/* Styles from deviceInfoPage */
.info-section {
  background: #1e1e1e;
  border-radius: 15px;
  padding: 30px;
  border: 2px solid #404040;
  margin-bottom: 25px;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3);
}
.param-block {
  display: flex;
  flex-direction: column;
  gap: 25px;
  margin-top: 20px;
}
.param-category {
  background: linear-gradient(135deg, #2a2a2a 0%, #252525 100%);
  border-radius: 12px;
  padding: 20px;
  border: 1px solid #555;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.2);
}
.category-title {
  color: #00bcd4;
  font-size: 1.2em;
  font-weight: 600;
  margin: 0 0 15px 0;
  padding-bottom: 8px;
  border-bottom: 2px solid #00bcd4;
  display: flex;
  align-items: center;
  gap: 8px;
}
.param-list {
  display: flex;
  flex-direction: column;
  gap: 8px;
}
.param-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 8px 12px;
  background: rgba(255, 255, 255, 0.05);
  border-radius: 6px;
  border-left: 3px solid #00bcd4;
  transition: all 0.2s ease;
}
.param-row:hover {
  background: rgba(0, 188, 212, 0.1);
  transform: translateX(3px);
}
.param-name {
  color: #e0e0e0;
  font-weight: 500;
  min-width: 160px;
}
.param-value {
  color: #00bcd4;
  font-weight: 400;
  text-align: right;
  font-family: 'Courier New', monospace;
}
.label {
  color: #00bcd4;
  font-size: 1.3em;
  font-weight: bold;
  margin-bottom: 10px;
  text-transform: uppercase;
  letter-spacing: 1px;
}
@media (max-width: 768px) {
  .param-row {
    flex-direction: column;
    align-items: flex-start;
    gap: 4px;
  }
  .param-name {
    min-width: auto;
    font-size: 0.9em;
  }
  .param-value {
    text-align: left;
    font-size: 0.9em;
  }
  .category-title {
    font-size: 1.1em;
  }
}

/* Styles from scpiPage */
.command-table {
  width: 100%;
  border-collapse: collapse;
  margin-top: 15px;
}
.command-table th, .command-table td {
  padding: 12px;
  text-align: left;
  border-bottom: 1px solid #555;
}
.command-table th {
  background: #2a2a2a;
  color: #00bcd4;
  font-weight: bold;
}
.command-table td {
  background: #1a1a1a;
}
.command-table tr:hover td {
  background: #333;
}
.terminal-section {
  background: #1e1e1e;
  border-radius: 10px;
  padding: 25px;
  border: 2px solid #404040;
}
.terminal-output {
  height: 200px;
  font-size: 0.9em;
  white-space: pre-wrap;
  background: #252525;
  border: 1px solid #555;
  border-radius: 5px;
  padding: 15px;
  margin-top: 20px;
  color: #00bcd4;
  font-family: monospace;
  overflow-y: auto;
}
.copy-icon {
  color: #00bcd4;
  cursor: pointer;
  font-size: 0.9em;
  margin-left: 8px;
  padding: 2px 4px;
  border-radius: 3px;
  transition: all 0.2s ease;
  opacity: 0.7;
}
.copy-icon:hover {
  background: #00bcd4;
  color: #1a1a1a;
  opacity: 1;
  transform: scale(1.1);
}

.terminal-buttons {
  display: flex;
  gap: 10px;
  margin-top: 15px;
}

/* Styles from adcPage */
.adc-container {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 20px;
  margin-bottom: 20px;
}
.adc-section {
  background: #1e1e1e;
  border-radius: 12px;
  padding: 20px;
  border: 3px solid #404040;
}
.section-header {
  text-align: center;
  margin-bottom: 20px;
  padding-bottom: 10px;
  border-bottom: 2px solid #404040;
}
.section-title {
  color: #ff9800;
  font-size: 1.5em;
  margin: 0;
  text-shadow: 0 0 10px rgba(255, 152, 0, 0.3);
}
.control-group {
  background: #252525;
  border-radius: 8px;
  padding: 15px;
  margin-bottom: 15px;
  border: 1px solid #555;
}
.control-label {
  color: #bbb;
  font-size: 0.9em;
  margin-bottom: 8px;
  display: block;
  font-weight: bold;
}
.select-wrapper {
  position: relative;
  margin-bottom: 15px;
}
select {
  width: 100%;
  padding: 10px;
  background: #1a1a1a;
  border: 2px solid #555;
  border-radius: 6px;
  color: #e0e0e0;
  font-size: 1em;
  cursor: pointer;
  transition: all 0.3s ease;
}
select:focus {
  border-color: #00bcd4;
  outline: none;
  box-shadow: 0 0 8px rgba(0, 188, 212, 0.3);
}
.measurement-display {
  background: #1a1a1a;
  border-radius: 8px;
  padding: 20px;
  text-align: center;
  border: 2px solid #00bcd4;
  box-shadow: 0 0 15px rgba(0, 188, 212, 0.2);
  margin-bottom: 15px;
}
.measurement-label {
  color: #888;
  font-size: 0.9em;
  margin-bottom: 10px;
}
.measurement-value {
  font-size: 3em;
  font-weight: bold;
  color: #4caf50;
  text-shadow: 0 0 20px rgba(76, 175, 80, 0.5);
  margin: 10px 0;
  font-family: 'Courier New', monospace;
}
.measurement-unit {
  font-size: 0.5em;
  color: #bbb;
  margin-left: 5px;
}
.measurement-raw {
  color: #888;
  font-size: 0.85em;
  margin-top: 8px;
  font-family: 'Courier New', monospace;
}
.measure-button {
  width: 100%;
  padding: 12px;
  background: linear-gradient(45deg, #00bcd4, #26c6da);
  color: white;
  border: none;
  border-radius: 8px;
  font-size: 1.1em;
  font-weight: bold;
  cursor: pointer;
  transition: all 0.3s ease;
  box-shadow: 0 3px 12px rgba(0, 188, 212, 0.3);
}
.measure-button:hover {
  background: linear-gradient(45deg, #26c6da, #00bcd4);
  box-shadow: 0 5px 16px rgba(0, 188, 212, 0.4);
  transform: translateY(-2px);
}
.measure-button:active {
  transform: translateY(0);
}
.info-box {
  background: #252525;
  border-radius: 8px;
  padding: 15px;
  border: 1px solid #555;
  margin-bottom: 15px;
}
.info-title {
  color: #00bcd4;
  font-size: 1.1em;
  margin-bottom: 10px;
  font-weight: bold;
}
.info-row {
  display: flex;
  justify-content: space-between;
  padding: 6px 0;
  border-bottom: 1px solid #333;
}
.info-row:last-child {
  border-bottom: none;
}
.info-label {
  color: #888;
}
.info-value {
  color: #e0e0e0;
  font-weight: bold;
}
.command-code {
  font-family: 'Courier New', monospace;
  color: #4caf50;
  background: #1a1a1a;
  padding: 3px 8px;
  border-radius: 4px;
  font-size: 0.95em;
}
@media (max-width: 1200px) {
  .adc-container {
    grid-template-columns: 1fr;
  }
}

/* Styles from info.html details */
details {
    background: #1e1e1e;
    border-radius: 12px;
    border: 2px solid #404040;
    margin-bottom: 15px;
    box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3);
}
summary {
    padding: 20px;
    font-size: 1.3em;
    font-weight: bold;
    color: #00bcd4;
    cursor: pointer;
    outline: none;
    user-select: none;
}
details[open] summary {
    border-bottom: 2px solid #404040;
}
.details-content {
    padding: 20px;
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
    gap: 20px;
}

/* System Setup Styles */
.setup-group {
  background: #1e1e1e;
  border-radius: 8px;
  border: 1px solid #404040;
  padding: 20px;
  margin-bottom: 20px;
}

.setup-heading {
  color: #00bcd4;
  font-size: 1.2em;
  margin: 0 0 15px 0;
  padding-bottom: 10px;
  border-bottom: 2px solid #404040;
}

.setup-row {
  display: flex;
  align-items: center;
  margin-bottom: 15px;
  gap: 15px;
  flex-wrap: wrap;
}

.setup-label {
  min-width: 180px;
  color: #aaa;
  font-weight: 500;
}

.setup-controls {
  display: flex;
  align-items: center;
  gap: 10px;
  flex: 1;
  flex-wrap: wrap;
}

.datetime-input {
  background: #252525;
  color: #e0e0e0;
  border: 2px solid #404040;
  border-radius: 6px;
  padding: 8px 12px;
  font-size: 14px;
  outline: none;
  transition: border-color 0.3s;
}

.datetime-input:focus {
  border-color: #00bcd4;
}

.text-input {
  background: #252525;
  color: #e0e0e0;
  border: 2px solid #404040;
  border-radius: 6px;
  padding: 8px 12px;
  font-size: 14px;
  width: 300px;
  outline: none;
  transition: border-color 0.3s;
}

.text-input:focus {
  border-color: #00bcd4;
}

.number-input {
  background: #252525;
  color: #e0e0e0;
  border: 2px solid #404040;
  border-radius: 6px;
  padding: 8px 12px;
  font-size: 14px;
  width: 80px;
  outline: none;
  transition: border-color 0.3s;
}

.number-input:focus {
  border-color: #00bcd4;
}

.select-input {
  background: #252525;
  color: #e0e0e0;
  border: 2px solid #404040;
  border-radius: 6px;
  padding: 8px 12px;
  font-size: 14px;
  width: 300px;
  outline: none;
  cursor: pointer;
  transition: border-color 0.3s;
}

.select-input:focus {
  border-color: #00bcd4;
}

.checkbox-input {
  width: 20px;
  height: 20px;
  cursor: pointer;
  accent-color: #00bcd4;
}

.checkbox-label {
  color: #aaa;
  margin-left: 5px;
  font-size: 14px;
}

.set-button {
  background: #00bcd4;
  color: #000;
  border: none;
  border-radius: 6px;
  padding: 8px 20px;
  font-size: 14px;
  font-weight: bold;
  cursor: pointer;
  transition: all 0.3s;
  text-transform: uppercase;
}

.set-button:hover {
  background: #00acc1;
  box-shadow: 0 4px 12px rgba(0, 188, 212, 0.4);
}

.set-button:active {
  transform: scale(0.98);
}

.slider-input {
  flex: 1;
  min-width: 150px;
  height: 6px;
  background: #404040;
  border-radius: 3px;
  outline: none;
  -webkit-appearance: none;
  appearance: none;
}

.slider-input::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 18px;
  height: 18px;
  background: #00bcd4;
  border-radius: 50%;
  cursor: pointer;
  transition: all 0.3s;
}

.slider-input::-webkit-slider-thumb:hover {
  background: #00acc1;
  box-shadow: 0 0 10px rgba(0, 188, 212, 0.6);
}

.slider-input::-moz-range-thumb {
  width: 18px;
  height: 18px;
  background: #00bcd4;
  border: none;
  border-radius: 50%;
  cursor: pointer;
  transition: all 0.3s;
}

.slider-input::-moz-range-thumb:hover {
  background: #00acc1;
  box-shadow: 0 0 10px rgba(0, 188, 212, 0.6);
}

.slider-value {
  min-width: 45px;
  color: #00bcd4;
  font-weight: bold;
  font-size: 16px;
  text-align: center;
}

.color-input {
  width: 60px;
  height: 40px;
  border: 2px solid #404040;
  border-radius: 6px;
  cursor: pointer;
  background: #252525;
  outline: none;
  transition: border-color 0.3s;
}

.color-input:focus {
  border-color: #00bcd4;
}

.variable-hint {
  color: #888;
  font-size: 12px;
  font-style: italic;
  margin-left: 10px;
}

.sensor-table {
  width: 100%;
  border-collapse: collapse;
  margin-bottom: 15px;
}

.sensor-table tr {
  border-bottom: 1px solid #404040;
}

.sensor-table tr:last-child {
  border-bottom: none;
}

.sensor-control {
  padding: 10px 15px 10px 0;
  vertical-align: top;
  white-space: nowrap;
}

.sensor-control .setup-label {
  margin-left: 10px;
  cursor: pointer;
}

.sensor-description {
  padding: 10px 0;
  vertical-align: top;
  width: 100%;
}

.sensor-description .variable-hint {
  display: block;
  margin-left: 0;
  line-height: 1.6;
}

@media (max-width: 768px) {
  .setup-row {
    flex-direction: column;
    align-items: flex-start;
  }
  
  .setup-label {
    min-width: 100%;
  }
  
  .setup-controls {
    width: 100%;
  }
}
)rawliteral";