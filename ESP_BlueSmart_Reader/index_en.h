/***************************************************************************
  Copyright (c) 2023 WASN.eu

  Licensed under the MIT License. You may not use this file except in
  compliance with the License. You may obtain a copy of the License at

  https://opensource.org/licenses/MIT

***************************************************************************/

const char HEADER_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="author" content="(c) 2023 WASN.eu">
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no"/>
<title>Victron BlueSmart IP22 __SYSTEMID__</title>
<link rel="stylesheet" type="text/css" href="/index.css"/>
)=====";


const char MAIN_html[] PROGMEM = R"=====(
<script>
var thresholdCalculation = 0;
var thresholdSaved = 1;
var pulseThreshold = -1;
var currentPower = -2;
var totalCounter = 0;
var messageShown = 0;
var msgType = 0;
var msgTimeout = 0;

function refreshDevTime() {
  var datetime = "";
  var now = new Date();
  datetime = now.toISOString().split('T')[0] + " " + (now.getHours() < 10 ? '0' : '') + now.getHours() + ":" + (now.getMinutes() < 10 ? '0' : '') + now.getMinutes() + ":" + (now.getSeconds() < 10 ? '0' : '') + now.getSeconds();
  document.getElementById('theTime').innerHTML = datetime;        
}

setInterval(function() {
  getReadings();
  updateUI();
}, 1000);

function updateUI() {
  if (msgType != 0)
    showMessage(msgType, msgTimeout);
  if (thresholdCalculation || (!thresholdSaved && pulseThreshold > 0)) {
    document.getElementById("tr3").style.display = "table-row";
    document.getElementById("tr4").style.display = "table-row";
    document.getElementById("tr5").style.display = "table-row";
    document.getElementById("calcThreshold").style.display = "none";
    document.getElementById("saveThreshold").style.display = "inline-grid";
  } else if (thresholdCalculation && thresholdSaved && pulseThreshold > 0) {
    document.getElementById("tr3").style.display = "table-row";
    document.getElementById("tr4").style.display = "table-row";
    document.getElementById("tr5").style.display = "table-row";
    document.getElementById("calcThreshold").style.display = "none";
    document.getElementById("saveThreshold").style.display = "none";
  } else if (messageShown == 0) {
    if (pulseThreshold == 0) {
      showMessage("invalidThreshold", 0);
    }
    document.getElementById("tr3").style.display = "none";
    document.getElementById("tr4").style.display = "none";
    document.getElementById("tr5").style.display = "none";
    document.getElementById("calcThreshold").style.display = "inline-grid";
    document.getElementById("saveThreshold").style.display = "none";
  } 

function hideMessages() {
  document.getElementById("message").style.display = "none";
  document.getElementById("invalidThreshold").style.display = "none";
  document.getElementById("thresholdCalculation").style.display = "none";
  document.getElementById("thresholdReset").style.display = "none";
  document.getElementById("thresholdSaved").style.display = "none";
  document.getElementById("thresholdFound").style.display = "none";
  document.getElementById("thresholdFailed").style.display = "none";
  document.getElementById("mqttConnFailed").style.display = "none";
  document.getElementById("publishFailed").style.display = "none";
  document.getElementById("restartSystem").style.display = "none";
  document.getElementById("publishData").style.display = "none";
  messageShown = 0;
}

function showMessage(message, timeout) {
  hideMessages();
  document.getElementById("message").style.display = "block";
  document.getElementById(message).style.display = "block";
  if (timeout > 0)
    setTimeout(hideMessages, timeout-50);
  messageShown = 1;
}

function restartSystem() {
  var xhttp = new XMLHttpRequest();
  if (confirm("Really restart system?")) {
    setTimeout(function(){location.href='/';}, 15000);
    showMessage("restartSystem", 15000);
  	xhttp.open("GET", "restart", true);
  	xhttp.send();
  }
}

function getReadings() {
  var xhttp = new XMLHttpRequest();
  var json;
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      json = JSON.parse(xhttp.responseText);
      msgType = 0;
      if ("msgType" in json && "msgTimeout" in json) {
        msgType = json.msgType;
        msgTimeout = (json.msgTimeout * 1000);
      }
      document.getElementById("voltage").innerHTML = json.voltage;
      document.getElementById("current").innerHTML = json.current;
      document.getElementById("Runtime").innerHTML = json.runtime;
      document.getElementById("RSSI").innerHTML = json.rssi;
   	} 
  };
  xhttp.open("GET", "readings?local=true", true);
  xhttp.send();
}

setInterval(refreshDevTime, 1000);
</script>
</head>

<body onload="hideMessages(); getReadings(); updateUI();">
<font face="Arial">
    <!-- <div class="dev-header"> -->
    <div class="headerrow">
        <div class="headercolumnbig" id="sysName">&nbsp; Victron BlueSmart IP22 &nbsp; &nbsp;</div>
        <div class="headercolumn" id="devName">__SYSTEMID__ (__LOCALIP__)</div>
        <div class="headercolumnshort">Version: &nbsp;</div><div class="headercolumn" id="devVersion">__FIRMWARE__</div>
        <div class="headercolumnshort">System-Time: &nbsp;</div><div class="headercolumn" id='theTime'>[00:00:00]</div>
    </div>
    </font>
    <div id="displayMainPage">
      <div class="nav-container">
        <div class='nav-left'>
          <a href="/config" class="nav-item tabButton">Settings</a>
          <a href="/update" class="nav-item tabButton">Update firmware</a>
          <a class="nav-item tabButtonred" onclick="restartSystem()">Restart system</a>
        </div>
      </div>
      <br/>
      <div id="mainPage" style="padding-left:10px;">

<div id="readings" style="margin-top:5px;">

<div class="Modbusmonrow">
	<div class="Modbusmoncolumn1">Battery Voltage:</div>
  <div class="Modbusmoncolumn2"><span id="voltage">__VOLTAGE__</span></div>
  <div class="Modbusmoncolumn3"> V</div>
</div>
<div class="Modbusmonrow">
	<div class="Modbusmoncolumn1" id="tr1">Charging Current:</div>
  <div class="Modbusmoncolumn2"><span id="current">__CURRENT__</span></div>
  <div class="Modbusmoncolumn3"> A</div>
</div>
<div class="Modbusmonrow">
	<div class="Modbusmoncolumn1">Runtime:</div>
  <div class="Modbusmoncolumn2"><span id="Runtime">-d -h -m</span></div>
  <div class="Modbusmoncolumn3"> </div>
</div>
<div class="Modbusmonrow">  
  <div class="Modbusmoncolumn1">WiFi RSSI:</div>
  <div class="Modbusmoncolumn2"><span id="RSSI">--</span></div>
  <div class="Modbusmoncolumn3"> dBm</div>
</div>

</div>
)=====";


const char EXPERT_html[] PROGMEM = R"=====(
<script>
var messageShown = 0;

function hideMessages() {
  document.getElementById("message").style.display = "none";
  document.getElementById("configSaved").style.display = "none";
  document.getElementById("restartSystem").style.display = "none";
  document.getElementById("resetSystem").style.display = "none";
  messageShown = 0;
}

function showMessage(message, timeout) {
  hideMessages();
  document.getElementById("message").style.display = "block";
  document.getElementById(message).style.display = "block";
  document.getElementById("heading").scrollIntoView();
  setTimeout(hideMessages, timeout);
  messageShown = 1;
}

function restartSystem() {
  var xhttp = new XMLHttpRequest();
  setTimeout(function(){location.href='/';}, 15000);
  showMessage("restartSystem", 15000);
  xhttp.open("GET", "restart", true);
  xhttp.send();
}

function resetSystem() {
  if (confirm("Reset settings and restart system?")) {
    var xhttp = new XMLHttpRequest();
    setTimeout(function(){location.href='/';}, 15000);
    showMessage("resetSystem", 15000);
    xhttp.open("GET", "reset", true);
    xhttp.send();
  }
}

function confirmRestart() {
    if (confirm("Save settings and restart system?"))
        return true;
    return false;
}

function configSaved() {
  const url = new URLSearchParams(window.location.search);
    if (url.has("saved")) {
      showMessage("configSaved", 2500)
      setTimeout(restartSystem, 3000);
    }
}

function refreshDevTime() {
  var datetime = "";
  var now = new Date();
  datetime = now.toISOString().split('T')[0] + " " + (now.getHours() < 10 ? '0' : '') + now.getHours() + ":" + (now.getMinutes() < 10 ? '0' : '') + now.getMinutes() + ":" + (now.getSeconds() < 10 ? '0' : '') + now.getSeconds();
  document.getElementById('theTime').innerHTML = datetime;        
}
setInterval(refreshDevTime, 1000);
</script>
</head>
<body onload="hideMessages(); configSaved();">
<font face="Arial">
    <!-- <div class="dev-header"> -->
    <div class="headerrow">
        <div class="headercolumnbig" id="sysName">&nbsp; Victron SmartBlue IP22 &nbsp; &nbsp;</div>
        <div class="headercolumn" id="devName">__SYSTEMID__ (__LOCALIP__)</div>
        <div class="headercolumnshort">Version: &nbsp;</div><div class="headercolumn" id="devVersion">__FIRMWARE__</div>
        <div class="headercolumnshort">System-Time: &nbsp;</div><div class="headercolumn" id='theTime'>[00:00:00]</div>
    </div>
    </font>
    <div id="displayMainPage">
      <div class="nav-container">
        <div class='nav-left'>
          <a href="/" class="nav-item tabButton">Main page</a>
          <a href="/config" class="nav-item tabButton">Back</a>
          <a onclick="resetSystem();" class="nav-item tabButton">Reset system settings</a>
        </div>
      </div>
      <br/>
      <div id="mainPage" style="padding-left:10px;">
<h2 id="heading">Expert Settings</h2>

<div style="max-width:335px;margin-top:10px;">
<form method="POST" action="/expert" onsubmit="return confirmRestart();">
  <fieldset><legend><b>&nbsp;nothing here &nbsp;</b></legend>
  </fieldset>
  <br />

  <fieldset><legend><b>&nbsp;InfluxDB&nbsp;</b></legend>
  <p><input id="checkbox_influxdb" name="influxdb" type="checkbox" __INFLUXDB__><b>Stream raw sensor data</b></p>
  </fieldset>

  <br />
  <p><button class="tabButtonred" type="submit">Save and restart</button></p>
</form>
</div>
)=====";


const char CONFIG_html[] PROGMEM = R"=====(
<script>
var messageShown = 0;
function hideMessages() {
  document.getElementById("message").style.display = "none";
  document.getElementById("configSaved").style.display = "none";
  document.getElementById("counterError").style.display = "none";
  document.getElementById("kwhError").style.display = "none";
  document.getElementById("mqttError").style.display = "none";
  document.getElementById("mqttAuthError").style.display = "none";
  document.getElementById("restartSystem").style.display = "none";
  messageShown = 0;
}
function showMessage(message, timeout) {
  hideMessages();
  document.getElementById("message").style.display = "block";
  document.getElementById(message).style.display = "block";
  document.getElementById("heading").scrollIntoView();
  setTimeout(hideMessages, timeout);
  messageShown = 1;
}
function restartSystem() {
  var xhttp = new XMLHttpRequest();
  setTimeout(function(){location.href='/';}, 15000);
  showMessage("restartSystem", 15000);
  xhttp.open("GET", "restart", true);
  xhttp.send();
}
function configSaved() {
  const url = new URLSearchParams(window.location.search);
  if (url.has("saved"))
    showMessage("configSaved", 4000);
}
function digitsOnly(input) {
  var regex = /[^0-9]/g;
  input.value = input.value.replace(regex, "");
}
function checkInput() {
  var err = 0;
  var height = 0;
  var xhttp = new XMLHttpRequest();
  if (document.getElementById("input_kwh_turns").value.length < 2 ||
    Number(document.getElementById("input_kwh_turns").value) < 50 ||
    Number(document.getElementById("input_kwh_turns").value) > 800) {
    document.getElementById("kwhError").style.display = "block";
    err++;
  }
  if (document.getElementById("input_consumption_kwh").value.length < 3 ||
    Number(document.getElementById("input_consumption_kwh").value) > 999999) {
    document.getElementById("counterError").style.display = "block";
    err++;
  }
  if (document.getElementById("checkbox_mqtt").checked == true &&
    (document.getElementById("input_mqttbroker").value.length < 4 ||
     document.getElementById("input_mqttbasetopic").value.length < 4 ||
     document.getElementById("input_mqttport").value.length < 4)) {
    document.getElementById("mqttError").style.display = "block";
    err++;
  }
  if (document.getElementById("checkbox_mqttauth").checked == true && 
    (document.getElementById("input_mqttuser").value.length < 4 ||
     document.getElementById("input_mqttpassword").value.length < 4)) {
    document.getElementById("mqttAuthError").style.display = "block";
    err++;
  }
  if (err > 0) {
    height = (err * 12) + 4;
    document.getElementById("message").style.height = height + "px";
    document.getElementById("message").style.display = "block";
    document.getElementById("heading").scrollIntoView();
    setTimeout(hideMessages, 4000);
    return false;
  } else {
    document.getElementById("message").style.display = "none";
    return true;
  }
}
function digitsOnly(input) {
  var regex = /[^0-9]/g;
  input.value = input.value.replace(regex, "");
}
function floatsOnly(input) {
  var regex = /[^0-9\.]/g;
  input.value = input.value.replace(/,/, ".").replace(regex, "");
}
function ASCIIOnly(input) {
  var regex = /[^0-9A-Z/_\-\.]/ig;
  input.value = input.value.replace(regex, "")
}
function toggleMQTT() {
  if (document.getElementById("checkbox_mqtt").checked == true) {
    document.getElementById("mqtt").style.display = "block";
  } else {
    document.getElementById("mqtt").style.display = "none";
  }
}
function toggleMQTTAuth() {
  if (document.getElementById("checkbox_mqttauth").checked == true) {
    document.getElementById("mqttauth").style.display = "block";
  } else {
    document.getElementById("mqttauth").style.display = "none";
  }
}
function toggleMQTTSecure() {
  if (document.getElementById("checkbox_mqtt_secure").checked == true) {
    document.getElementById("input_mqttport").value = 8883;
  } else {
    document.getElementById("input_mqttport").value = 1883;
  }
}
function togglePower() {
  if (document.getElementById("checkbox_power").checked == true) {
    document.getElementById("power").style.display = "block";
  } else {
    document.getElementById("power").style.display = "none";
  }
}
function togglePowerAvg() {
  if (document.getElementById("checkbox_power_avg").checked == true) {
    document.getElementById("power_avg").style.display = "block";
  } else {
    document.getElementById("power_avg").style.display = "none";
  }
}
function togglePowerSaving(warning) {
  if (document.getElementById("checkbox_powersaving").checked == true) {
    if (warning)
      alert('WARNING: The web interface is only accessible for 5 minutes after activating '+
        'the power saving mode. The Wifi connection is switched off and then only activated '+
        'for a few seconds to publish data via MQTT. To regain access to the web '+
        'interface you need to send a MQTT message with retain '+
        'flag set (__MQTT_BASE_TOPIC__/__SYSTEMID__/cmd/powersave 0) to the device.');
      document.getElementById("checkbox_power_avg").checked = true;
      document.getElementById("input_power_avg_secs").value = __POWER_AVG_SECS_POWERSAVING__;
      togglePowerAvg();
    if (Number(document.getElementById("input_mqttinterval").value) < __MQTT_INTERVAL_MIN_POWERSAVING__)
        document.getElementById("input_mqttinterval").value = __MQTT_INTERVAL_MIN_POWERSAVING__;
  }
}
function toggleJSON() {
  if (document.getElementById("checkbox_mqtt_json").checked == false) {
    document.getElementById("checkbox_mqtt_ha_discovery").checked = false;
  }
}
function toggleHADiscovery() {
  if (document.getElementById("checkbox_mqtt_ha_discovery").checked == true) {
    document.getElementById("checkbox_mqtt_json").checked = true;
  }
}
function refreshDevTime() {
  var datetime = "";
  var now = new Date();
  datetime = now.toISOString().split('T')[0] + " " + (now.getHours() < 10 ? '0' : '') + now.getHours() + ":" + (now.getMinutes() < 10 ? '0' : '') + now.getMinutes() + ":" + (now.getSeconds() < 10 ? '0' : '') + now.getSeconds();
  document.getElementById('theTime').innerHTML = datetime;        
}
setInterval(refreshDevTime, 1000);
</script>
</head>
<body onload="configSaved(); togglePower(); toggleMQTT(); toggleMQTTAuth(); toggleMQTTSecure(); togglePowerAvg(); togglePowerSaving(false); toggleJSON(); toggleHADiscovery();">
<font face="Arial">
    <!-- <div class="dev-header"> -->
    <div class="headerrow">
        <div class="headercolumnbig" id="sysName">&nbsp; Victron BlueSmart IP22 &nbsp; &nbsp;</div>
        <div class="headercolumn" id="devName">__SYSTEMID__ (__LOCALIP__)</div>
        <div class="headercolumnshort">Version: &nbsp;</div><div class="headercolumn" id="devVersion">__FIRMWARE__</div>
        <div class="headercolumnshort">System-Time: &nbsp;</div><div class="headercolumn" id='theTime'>[00:00:00]</div>
    </div>
    </font>
    <div id="displayMainPage">
      <div class="nav-container">
        <div class='nav-left'>
          <a href="/" class="nav-item tabButton">Main page</a>
          <a href="/nvsbackup" class="nav-item tabButton">Backup settings</a>
          <a href="/nvsimport" class="nav-item tabButton">Import settings</a>
          <a href="/expert" class="nav-item tabButton">Expert settings</a>          
        </div>
      </div>
      <br/>
      <div id="mainPage" style="padding-left:10px;">
<h2 id="heading">Settings</h2>
<div style="max-width:335px;margin-top:10px;">
<form method="POST" action="/config" onsubmit="return checkInput();">
  <fieldset><legend><b>&nbsp;Victron SmartBlue IP 22&nbsp;</b></legend>
  <p><b>Reader ID (optional)</b><br />
  <input id="input_meter_id" name="meter_id" size="16" maxlength="16" value="__METER_ID__" onkeyup="ASCIIOnly(this);"></p>
  <p><b>Auto-backup counter (__BACKUP_CYCLE_MIN__-__BACKUP_CYCLE_MAX__ min.)</b><br />
  <input id="input_backup_cycle" name="backup_cycle" size="16" maxlength="3" value="__BACKUP_CYCLE__" onkeyup="digitsOnly(this);"></p>
  </span>
  </fieldset>
  <br />
  <fieldset><legend><b>&nbsp;MQTT&nbsp;</b></legend>
  <p><input id="checkbox_mqtt" name="mqtt" onclick="toggleMQTT();" type="checkbox" __MQTT__><b>Enable</b></p>
  <span id="mqtt" style="display:none">
  <p><b>Hostname broker</b><br />
  <input id="input_mqttbroker" name="mqttbroker" size="16" maxlength="64" value="__MQTT_BROKER__" onkeyup="ASCIIOnly(this);"></p>
  <p><b>Port (default 1883)</b><br />
  <input id="input_mqttport" name="mqttport" size="16" maxlength="5" value="__MQTT_PORT__" onkeyup="digitsOnly(this);"></p>
  <p><b>Message topic</b><br />
  <input id="input_mqttbasetopic" name="mqttbasetopic" size="16" maxlength="64" value="__MQTT_BASE_TOPIC__" onkeyup="ASCIIOnly(this);"></p>
  <p><input id="checkbox_mqtt_json" name="mqtt_json" onclick="toggleJSON();" type="checkbox" __MQTT_JSON__><b>Publish data as JSON</b></p>
  <p><input id="checkbox_mqtt_ha_discovery" name="mqtt_ha_discovery" onclick="toggleHADiscovery();" type="checkbox" __MQTT_HA_DISCOVERY__><b>Home Assistant Discovery</b></p>
  <p><b>Publishing interval (sec.)</b><br />
  <input id="input_mqttinterval" name="mqttinterval" value="__MQTT_INTERVAL__" maxlength="4" onkeyup="digitsOnly(this);"></p>
  <p><input id="checkbox_powersaving" name="powersavingmode" type="checkbox" onclick="togglePowerSaving(true);" __POWER_SAVING_MODE__><b>Power saving mode</b></p>
  <p><input id="checkbox_mqttauth" name="mqttauth" onclick="toggleMQTTAuth();" type="checkbox" __MQTT_AUTH__><b>Enable authentication</b></p>
     <span style="display:none" id="mqttauth">
     <p><b>Username</b><br />
     <input id="input_mqttuser" name="mqttuser" size="16" maxlength="32" value="__MQTT_USERNAME__"></p>
     <p><b>Password</b><br />
     <input id="input_mqttpassword" type="password" name="mqttpassword" size="16" maxlength="32" value="__MQTT_PASSWORD__"></p>
     </span>
     <p><input id="checkbox_mqtt_secure" name="mqtt_secure" type="checkbox" onclick="toggleMQTTSecure();" __MQTT_SECURE__><b>Enable TLS</b></p>
    </span>
  </fieldset>
  <br />
  <p><button class="tabButtonred" type="submit">Save settings</button></p>
</form>
</div>
)=====";

const char UPDATE_html[] PROGMEM = R"=====(
<script>
function initPage() {
  document.getElementById('upload_form').onsubmit = function(e) {
    e.preventDefault();
    var uploadForm = new FormData();
    var firmwareFile = document.getElementById('firmware_file').files[0];
    if (!firmwareFile) {
      alert('Please select a firmware file *.bin for upload.');
      return false;
    }
    uploadForm.append("files", firmwareFile, firmwareFile.name);
    var xhttp = new XMLHttpRequest();
    xhttp.upload.addEventListener("progress", function(e) {
      if (e.lengthComputable) {
        var percent = Math.round((e.loaded/e.total)*100);
        var progress = document.getElementById('progress');
        if (percent == 100) {
            progress.value = percent;
            progress.style.display = "none";
            document.getElementById('install').style.display = "block";
        } else {
            progress.style.display = "block";
            progress.value = percent;
        }
      }
    }, false);
    xhttp.open("POST", "/update", true);
    xhttp.onload = function() {
      if (xhttp.status == 200) {
        location.href='/update?res=ok';
        return true;
      } else {
        location.href='/update?res=err';
        return false;
      }
    };
    xhttp.send(uploadForm);
  }
}

function refreshDevTime() {
  var datetime = "";
  var now = new Date();
  datetime = now.toISOString().split('T')[0] + " " + (now.getHours() < 10 ? '0' : '') + now.getHours() + ":" + (now.getMinutes() < 10 ? '0' : '') + now.getMinutes() + ":" + (now.getSeconds() < 10 ? '0' : '') + now.getSeconds();
  document.getElementById('theTime').innerHTML = datetime;        
}
setInterval(refreshDevTime, 1000);

</script>
</head>
<body onload="initPage();">
<font face="Arial">
    <!-- <div class="dev-header"> -->
    <div class="headerrow">
        <div class="headercolumnbig" id="sysName">&nbsp; Victron BlueSmart IP22 &nbsp; &nbsp;</div>
        <div class="headercolumn" id="devName">__SYSTEMID__ (__LOCALIP__)</div>
        <div class="headercolumnshort">Version: &nbsp;</div><div class="headercolumn" id="devVersion">__FIRMWARE__</div>
        <div class="headercolumnshort">System-Time: &nbsp;</div><div class="headercolumn" id='theTime'>[00:00:00]</div>
    </div>
    </font>
    <div id="displayMainPage">
      <div class="nav-container">
        <div class='nav-left'>
          <a href="/" class="nav-item tabButton">Back</a>
        </div>
      </div>
      <br/>
      <div id="mainPage" style="padding-left:10px;">
<h2>Firmware update</h2>
<progress id="progress" style="display:none;margin: 17px 0px 6px 50px;width:235px" value="0" max="100"></progress>
<div id="install" style="display:none;margin-top:10px;text-align:center;color:red"><strong>Installing update...</strong></div>

<div style="margin-left:40px;margin-bottom:10px">
1. Select firmware file *.bin<br>
2. Start update<br>
3. Upload takes about 20 sec.<br>
4. Restart system
</div>
<div>
<form method="POST" action="#" enctype="multipart/form-data" id="upload_form">
  <input id="firmware_file" type="file" accept=".bin" name="update">
  <p><button class="tabButtonred" type="submit">Start update</button></p>
</form>
</div>
)=====";


const char UPDATE_OK_html[] PROGMEM = R"=====(
<script>
var skip = 0

function restartSystem() {
  if (skip)
    return;
  if (confirm("Really restart system?")) {
    var xhttp = new XMLHttpRequest();
    setTimeout(function(){location.href='/';}, 15000);
    document.getElementById("message").style.display = "block";
    document.getElementById("restartSystem").style.display = "block";
    xhttp.open("GET", "restart", true);
    xhttp.send();
  }
}

function resetSystem() {
  if (confirm("Reset all settings and restart system?")) {
    var xhttp = new XMLHttpRequest();
    setTimeout(function(){location.href='/';}, 15000);
    document.getElementById("message").style.display = "block";
    document.getElementById("resetSystem").style.display = "block";
    xhttp.open("GET", "reset", true);
    xhttp.send();
    skip = 1;
  }
}

function refreshDevTime() {
  var datetime = "";
  var now = new Date();
  datetime = now.toISOString().split('T')[0] + " " + (now.getHours() < 10 ? '0' : '') + now.getHours() + ":" + (now.getMinutes() < 10 ? '0' : '') + now.getMinutes() + ":" + (now.getSeconds() < 10 ? '0' : '') + now.getSeconds();
  document.getElementById('theTime').innerHTML = datetime;        
}
setInterval(refreshDevTime, 1000);

</script>
</head>
<body>
<div style="text-align:left;display:inline-block;min-width:340px;">
<div style="text-align:center;">
<h2 id="heading">Firmware update<br />successful</h2>
</div>
<div>
<p><button class="tabButtonred" onclick="resetSystem(); restartSystem();">Restart system</button></p>
<p><button onclick="location.href='/';" class="tabButton>Main page</button></p>
</div>
)=====";


const char UPDATE_ERR_html[] PROGMEM = R"=====(
<script>
function refreshDevTime() {
  var datetime = "";
  var now = new Date();
  datetime = now.toISOString().split('T')[0] + " " + (now.getHours() < 10 ? '0' : '') + now.getHours() + ":" + (now.getMinutes() < 10 ? '0' : '') + now.getMinutes() + ":" + (now.getSeconds() < 10 ? '0' : '') + now.getSeconds();
  document.getElementById('theTime').innerHTML = datetime;        
}
setInterval(refreshDevTime, 1000);
</script>
</head>
<body>
<div style="text-align:left;display:inline-block;min-width:340px;">
<div style="text-align:center;">
<h2>Firmware update<br />failed</h2>
</div>
<div>
<p><button onclick="location.href='/update';" class="tabButtonred">Repeat update</button></p>
<p><button onclick="location.href='/';">Main page</button></p>
</div>
)=====";


const char IMPORT_html[] PROGMEM = R"=====(
<script>
function initPage() {
  document.getElementById('upload_form').onsubmit = function(e) {
    e.preventDefault();
    var uploadForm = new FormData();
    var configFile = document.getElementById('json_file').files[0];
    if (!configFile) {
      alert('Please select a JSON configuration file for upload.');
      return false;
    }
    uploadForm.append("files", configFile, configFile.name);
    var xhttp = new XMLHttpRequest();
    xhttp.upload.addEventListener("progress", function(e) {
      if (e.lengthComputable) {
        var percent = Math.round((e.loaded/e.total)*100);
        var progress = document.getElementById('progress');
        if (percent == 100) {
            progress.value = percent;
            progress.style.display = "none";
            document.getElementById('install').style.display = "block";
        } else {
            progress.style.display = "block";
            progress.value = percent;
        }
      }
    }, false);
    xhttp.open("POST", "/nvsimport", true);
    xhttp.onload = function() {
      if (xhttp.status == 200) {
        location.href='/nvsimport?res=ok';
        return true;
      } else {
        location.href='/nvsimport?res=err';
        return false;
      }
    };
    xhttp.send(uploadForm);
  }
}

function refreshDevTime() {
  var datetime = "";
  var now = new Date();
  datetime = now.toISOString().split('T')[0] + " " + (now.getHours() < 10 ? '0' : '') + now.getHours() + ":" + (now.getMinutes() < 10 ? '0' : '') + now.getMinutes() + ":" + (now.getSeconds() < 10 ? '0' : '') + now.getSeconds();
  document.getElementById('theTime').innerHTML = datetime;        
}
setInterval(refreshDevTime, 1000);
</script>
</head>
<body onload="initPage();">
<font face="Arial">
    <!-- <div class="dev-header"> -->
    <div class="headerrow">
        <div class="headercolumnbig" id="sysName">&nbsp; Victron BlueSmart IP22 &nbsp; &nbsp;</div>
        <div class="headercolumn" id="devName">__SYSTEMID__ (__LOCALIP__)</div>
        <div class="headercolumnshort">Version: &nbsp;</div><div class="headercolumn" id="devVersion">__FIRMWARE__</div>
        <div class="headercolumnshort">System-Time: &nbsp;</div><div class="headercolumn" id='theTime'>[00:00:00]</div>
    </div>
    </font>
    <div id="displayMainPage">
      <div class="nav-container">
        <div class='nav-left'>
          <a href="/" class="nav-item tabButton">Main page</a>
          <a href="/config" class="nav-item tabButton">Back</a>
        </div>
      </div>
      <br/>
      <div id="mainPage" style="padding-left:10px;">
<h2>Import system settings</h2>
<progress id="progress" style="display:none;margin: 17px 0px 6px 50px;width:235px" value="0" max="100"></progress>
<div id="install" style="display:none;margin-top:10px;text-align:center;color:red"><strong>Importing settings...</strong></div>

<div style="margin-left:50px;margin-bottom:10px">
1. Select JSON configuration file<br />
2. Start upload<br />
3. Restart system
</div>
<div>
<form method="POST" action="#" enctype="multipart/form-data" id="upload_form">
  <input id="json_file" type="file" accept=".json" name="update">
  <p><button class="tabButtonred" type="submit">Upload settings</button></p>
</form>
</div>
</div>
)=====";


const char IMPORT_OK_html[] PROGMEM = R"=====(
<script>
function restartSystem() {
  if (confirm("Really restart system?")) {
    var xhttp = new XMLHttpRequest();
    setTimeout(function(){location.href='/';}, 15000);
    document.getElementById("message").style.display = "block";
    document.getElementById("restartSystem").style.display = "block";
    xhttp.open("GET", "restart", true);
    xhttp.send();
  }
}
function refreshDevTime() {
  var datetime = "";
  var now = new Date();
  datetime = now.toISOString().split('T')[0] + " " + (now.getHours() < 10 ? '0' : '') + now.getHours() + ":" + (now.getMinutes() < 10 ? '0' : '') + now.getMinutes() + ":" + (now.getSeconds() < 10 ? '0' : '') + now.getSeconds();
  document.getElementById('theTime').innerHTML = datetime;        
}
setInterval(refreshDevTime, 1000);
</script>
</head>
<body>
<font face="Arial">
    <!-- <div class="dev-header"> -->
    <div class="headerrow">
        <div class="headercolumnbig" id="sysName">&nbsp; Victron BlueSmart IP22 &nbsp; &nbsp;</div>
        <div class="headercolumn" id="devName">__SYSTEMID__ (__LOCALIP__)</div>
        <div class="headercolumnshort">Version: &nbsp;</div><div class="headercolumn" id="devVersion">__FIRMWARE__</div>
        <div class="headercolumnshort">System-Time: &nbsp;</div><div class="headercolumn" id='theTime'>[00:00:00]</div>
    </div>
    </font>
    <div id="displayMainPage">
      <div class="nav-container">
        <div class='nav-left'>
          <a href="/" class="nav-item tabButton">Back</a>
          <a class="tabButtonred" onclick="restartSystem();">Restart system</a>
        </div>
      </div>
      <br/>
      <div id="mainPage" style="padding-left:10px;">
<h2 id="heading">Settings import<br />successful</h2>
</div>

)=====";


const char IMPORT_ERR_html[] PROGMEM = R"=====(
<script>
function refreshDevTime() {
  var datetime = "";
  var now = new Date();
  datetime = now.toISOString().split('T')[0] + " " + (now.getHours() < 10 ? '0' : '') + now.getHours() + ":" + (now.getMinutes() < 10 ? '0' : '') + now.getMinutes() + ":" + (now.getSeconds() < 10 ? '0' : '') + now.getSeconds();
  document.getElementById('theTime').innerHTML = datetime;        
}
setInterval(refreshDevTime, 1000);
</script>
</head>
<body>
<font face="Arial">
    <!-- <div class="dev-header"> -->
    <div class="headerrow">
        <div class="headercolumnbig" id="sysName">&nbsp; Victron BlueSmart IP22 &nbsp; &nbsp;</div>
        <div class="headercolumn" id="devName">__SYSTEMID__ (__LOCALIP__)</div>
        <div class="headercolumnshort">Version: &nbsp;</div><div class="headercolumn" id="devVersion">__FIRMWARE__</div>
        <div class="headercolumnshort">System-Time: &nbsp;</div><div class="headercolumn" id='theTime'>[00:00:00]</div>
    </div>
    </font>
    <div id="displayMainPage">
      <div class="nav-container">
        <div class='nav-left'>
          <a href="/" class="nav-item tabButton">Back</a>
          <a href="/nvsimport" class="tabButtonred">Repeat import</a>
        </div>
      </div>
      <br/>
      <div id="mainPage" style="padding-left:10px;">
<h2>Settings import<br />failed</h2>
</div>
)=====";


const char FOOTER_html[] PROGMEM = R"=====(
</div>

    <!-- Pin to bottom right corner -->
    <div class="bottom right-0">2023 &copy; <a href="https://www.wasn.eu" target="_new" style="text-decoration: none;color: white;">WASN.eu</a></div>

    <!-- Pin to bottom left corner -->
    <div class="bottom left-0">
      <div id="message" style="display:none;color:red;font-weight:bold;max-width:335px">
        <span id="invalidThreshold" style="display:none">Invalid Calibration value</span>
        <span id="thresholdCalculation" style="display:none">Calibrating sensor, please wait</span>
        <span id="thresholdFound" style="display:none;color:green">Calibrating sensor successful</span>
        <span id="thresholdReset" style="display:none;color:green">Counter reset successful</span>
        <span id="thresholdSaved" style="display:none;color:green">Calibration set successfully</span>
        <span id="thresholdFailed" style="display:none">Readings not sufficient</span>
        <span id="mqttConnFailed" style="display:none">MQTT connection failed</span>
        <span id="publishFailed" style="display:none">Failed to publish data via MQTT</span>
        <span id="restartSystem" style="display:none">System will restart shortly...</span>
        <span id="publishData" style="display:none;color:green">Published data via MQTT</span>
        <span id="configSaved" style="display:none;color:green">Settings saved</span>
        <span id="restartSystem" style="display:none;color:red">System will restart shortly...</span>
        <span id="resetSystem" style="display:none">After system reset connect to local access point to configure system!</span>
        <span id="configSaved" style="display:none;color:green">Saved setttings</span>
        <span id="counterError" style="display:none">Check counter reading</span>
        <span id="kwhError" style="display:none">Check value for rotations/kWh</span>
        <span id="mqttError" style="display:none">Check MQTT settings</span>
        <span id="mqttAuthError" style="display:none">Check MQTT username/password</span>
        <span id="restartSystem" style="display:none;color:red">System will restart shortly...</span>
        <span id="restartSystem" style="display:none">System will restart shortly...</span>
        <span id="resetSystem" style="display:none">After system reset connect to local access point to configure system!</span>
        <span id="restartSystem" style="display:none">System will restart shortly...</span>
        <noscript>Please enable JavaScript!</noscript>
      </div>
    </div>

</body>
</html>
)=====";
