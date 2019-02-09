
var xmlhttp;
if (window.XMLHttpRequest) {
    xmlhttp = new XMLHttpRequest();
} else {
    xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
}
xmlhttp.onreadystatechange = function () {
    var btnLed = document.getElementById("btnLed");
    var lang = document.getElementById("language");
    var color = document.getElementById("RGBColor");
    if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
        var obj = JSON.parse(xmlhttp.responseText);
        if(btnLed) {
            setLEDButton(document.getElementById("btnLed"), obj["ledstate"]);
        }
        if(lang) {
            for (var i = 0; i < lang.length; i++) {
                if (obj["language"] == lang.options[i].value) {
                    lang.options[i].selected = "selected";
                }
            }
        }
        if(color) {
            color.value = obj["RGBColor"];
        }
        // showReturnMsg("Success","初始化完成");
    }
    else if (xmlhttp.readyState == 4) showReturnMsg("Error", "初始化失败");
};
// showReturnMsg("Info", "正在进行页面初始化...");
xmlhttp.open("GET", "Init", true);
xmlhttp.send();

function setLEDButton(btnLed, stateValue) {
    btnLed.className = stateValue == "on" ? "ledon" : "ledoff";
    btnLed.value = stateValue == "on" ? "关闭板载LED灯" : "打开板载LED灯";
};
