
#ifndef INDEX_HTML_H
#define INDEX_HTML_H
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