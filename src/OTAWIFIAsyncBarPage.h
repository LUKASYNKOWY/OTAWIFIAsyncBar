#ifndef OTAWIFIAsyncBarPage_h
#define OTAWIFIAsyncBarPage_h

const char* OTAWIFIAsyncBarPage=R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 OTA Update</title>
<style>
body{font-family:Arial;background:#f2f2f2;margin:0;padding:30px}
.container{max-width:500px;margin:auto;background:white;padding:25px;border-radius:10px}
h1{margin-top:0}
input{width:100%;margin:15px 0}
button{width:100%;padding:12px;background:#1976d2;color:white;border:0;border-radius:5px}
button:disabled{background:#888}
.progress{margin-top:20px;background:#ddd;height:25px}
.bar{height:100%;width:0;background:#1976d2;color:white;text-align:center;line-height:25px}
#status{text-align:center;margin-top:15px}
</style>
</head>
<body>
<div class="container">
<h1>ESP32 OTA Update</h1>
<form id="form">
<input type="file" name="update" accept=".bin" required>
<button id="button" type="submit">Update firmware</button>
</form>
<div id="status">Ready</div>
<div class="progress">
<div id="bar" class="bar">0%</div>
</div>
</div>
<script>
const form=document.getElementById("form");
const button=document.getElementById("button");
const bar=document.getElementById("bar");
const status=document.getElementById("status");

form.addEventListener("submit",function(e){
e.preventDefault();

const data=new FormData(form);
const request=new XMLHttpRequest();

button.disabled=true;
status.innerHTML="Updating...";

request.open("POST","/update");

request.upload.addEventListener("progress",function(e){
if(e.lengthComputable){
const percent=Math.round((e.loaded/e.total)*100);
bar.style.width=percent+"%";
bar.innerHTML=percent+"%";
}
});

request.addEventListener("load",function(){
if(request.status==200){
bar.style.width="100%";
bar.innerHTML="100%";
status.innerHTML="Update complete. Restarting...";
}else{
status.innerHTML="Update failed.";
button.disabled=false;
}
});

request.addEventListener("error",function(){
status.innerHTML="Connection error.";
button.disabled=false;
});

request.send(data);
});
</script>
</body>
</html>
)rawliteral";

#endif
