function HTTPGET(url) {
    var req = new XMLHttpRequest();
    req.open("GET", url, false);
    req.send(null);
    return req.responseText;
}

Pebble.addEventListener("ready",
  function(e) {
    //App is ready to receive JS messages
    console.log("js ready");
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    //console.log("Als String: " +JSON.stringify(e.payload));
    //console.log("Typ" + e.type);
    //console.log("blub: " +e.payload.cmd);
    //console.log("on: " +e.payload.on);
    //console.log("off: " +e.payload.off);
    
    if (e.payload) 
    {
      //console.log("status: "+ e.payload.status + " device: "+ e.payload.device); 
      if (e.payload.device == 0)
        {
          HTTPGET("http://192.168.1.210/lib/powerpi.php?action=setsocket&socket=anlage_sub&status="+ e.payload.status + "/");
        }
      else
        {
          HTTPGET("http://192.168.1.210/lib/powerpi.php?action=setsocket&socket=lampe_wz&status="+ e.payload.status + "/");
        }

    }
  }
);