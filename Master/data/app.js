document.addEventListener('alpine:init', () => {
  Alpine.store('Ws', {
      etablished: false,

      toggle() {
          this.etablished = ! this.etablished
      }
  })
})
document.addEventListener('DOMContentLoaded',function (){
  var graphNiveauOption = {
    title: { text: 'Niveau VL53L1X' },
    // subtitle: {text: 'Using I2C Interface'},
    // chart: {
    //     type: 'area'
    // },
    time: {
      useUTC: false
    },
    legend: {
      enabled: true
    },
    rangeSelector: {
        buttons: [{
            count: 1,
            type: 'minute',
            text: '1M'
        }, {
            count: 5,
            type: 'minute',
            text: '5M'
        }, {
            count: 30,
            type: 'minute',
            text: '30M'
        }, {
            type: 'all',
            text: 'All'
        }],
        inputEnabled: false,
        selected: 0
    },
    title: {
        text: 'Niveau (%)'
    },
    plotOptions: {
      series: {
        
        showInNavigator: true
      },
      line: { 
        animation: false,
        dataLabels: { enabled: true }
      },
      // series: [
      //   { 
      //     color: '#059e8a',
      //     name: "Niveau" 
      //   },
      //   { 
      //     color: '#05918a',
      //     name: "Ouverture" 
      //   }
      // ]
    },
    xAxis: {
        //categories: [],
      type: 'datetime',
      dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
        title: {
            text: '(%)'
        }
    },
    series: [ {
      name: 'niveau (%)',
      data: [],
      color: '#05918a',
      
      },
      { 
        data:[],
        name: 'Ouverture (%)',
        color: '#FFAA8a' 
      },
      { 
        data:[],
        name: 'Cible (%)',
        color: '#FFC78A' 
      }//, {
    //       name: 'John',
    //       data: [5, 7, 3]
    //   }
    ]
  };
  chartNiveau = Highcharts.stockChart('chartNiveau',graphNiveauOption)
  chartNiveau.series[0].addPoint([(new Date()).getTime(), 10],true ,false,true);
})
var gateway = `ws://${window.location.hostname}/ws`;
//var gateway = `ws://192.168.1.24/ws`;
var websocket;
window.addEventListener('load', onLoad);
function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen    = onOpen;
  websocket.onclose   = onClose;
  websocket.onmessage = onMessage; // <-- add this line
  
}
function onOpen(event) {
  console.log('Connection opened');
  Alpine.store("Ws.etablished",true);
}
function onClose(event) {
  console.log('Connection closed');
  //alert('Connection closed');
  setTimeout(initWebSocket, 2000);
  Alpine.store("Ws.etablished",false);
}
function onMessage(event) {
  //console.log(event.data);
  var data = JSON.parse(event.data);
  var keys = Object.keys(data);
  for (let i = 0; i < keys.length; i++) {
    const element = keys[i];
    //console.log(element);
    var el  = document.getElementById(element);
    if(typeof(el) != 'undefined' && el != null){
      el.innerHTML = data[element];
    }else{
      console.log("element: " + element + " not exist");
    }
    
  }
}
function onLoad(event) {
  initWebSocket();
  initButton();

  var rangePosVanne = document.querySelector("#RangePosVanneTarget")

  //console.log(rangePosVanne);
  rangePosVanne.addEventListener('change',(e) => {
    console.log(e);
    websocket.send(e.target.id + " " + e.target.value)
  })

  var computedbtn = document.querySelectorAll(".computed")
  computedbtn.forEach(element => {
    element.addEventListener('click',(e) =>{
      console.log(e);
      websocket.send("Action:" + e.target.dataset.node + ":" + e.target.dataset.action +";")
    });
  
  })
  console.log("computed ", computedbtn);
}
function initButton() {
  document.getElementById('button').addEventListener('click', toggle);
}
function toggle(){
  // let message;
  // message["LED"] = document.querySelector("#LED").innerHTML;
  // message["POT"] = document.querySelector("#POT").innerHTML;
  // websocket.send(JSON.stringify(message));
  websocket.send("toggle");
}