class modes_C {
  constructor(name,id){
      this.name = name;
      this.id = id;
  }
  active(){
      console.log(`active function ${this.name} `);
      // document.querySelectorAll(`[data-moded="${this.id}"]`).forEach(el => {
      //     console.log(el);
      //     el.removeAttribute("disabled")
      // })
      document.querySelectorAll(`[data-modee="${this.id}"]`).forEach(el => {
          el.classList.remove('visually-hidden')
      })
  }
  desactive(){

      // document.querySelectorAll(`[data-moded="${this.id}"]`).forEach(el => {
      //     el.setAttribute("disabled",true)
      // })

      document.querySelectorAll(`[data-modee="${this.id}"]`).forEach(el => {
          el.classList.add('visually-hidden')
      })
  }
}
class modeBasic extends modes_C{
  constructor(name,id){
    super(name,id)
  }
  active(){
    super.active();
    try {
      chartNiveau.yAxis[0].addPlotBand({from: 20, to: 80, color: '#AAFFC5', id: 'plot-band-1' })
      
    } catch (error) {
      
    }
  }
  desactive(){
    super.desactive();
    try {
      chartNiveau.yAxis[0].removePlotBand('plot-band-1')
      
    } catch (error) {
      
    }
  }
}
let mode1 = new modes_C("mode0",0);
let mode2 = new modeBasic("mode1",1);
let mode3 = new modes_C("mode2",2);
let activeMode = 0;
var modes_li
var modes
var param
var modesArray = [mode1,mode2,mode3];

/**
 * 
 * @param {number} timeS Temps en secondes
 * @returns String
 */
function timeElapsedToString(timeS) {
  if (timeS<60) {
    return timeS + "s"
  }
  if (timeS < 60*60) {
    return timeS/60 + "min" + timeS%60;
  }
  return timeS/3600 + "h" + (timeS%3600)/60 + "min" + (timeS%3600)%60 + "s"
}

function desactivateModes() {
  modes_li.forEach(el => {
    el.classList.remove('active')
  })
}
function activateMode(el) {
  el.classList.add('active')
}
document.addEventListener('alpine:init', () => {
  Alpine.store('Ws', {
      etablished: false,

      toggle() {
          this.etablished = ! this.etablished
      }
  })
})

document.addEventListener('DOMContentLoaded',function (){
  modes = document.querySelector("#modes")
  modes_li =  modes.querySelectorAll("li")
  
  modesArray.forEach(el => {
    el.desactive()
  })
  modesArray[activeMode].active();

  modes_li.forEach(el => {
    el.addEventListener('click', el => {
      
      
      let url = "/mode?modeNum="+ el.target.dataset.num

      fetch(url).then(response => {
        desactivateModes();

        modesArray[activeMode].desactive()

        activateMode(el.target)
        activeMode = el.target.dataset.num
        modesArray[activeMode].active()
      })//.catch(console.error("erreur"))
    })
  })

  var graphNiveauOption = {
    title: { text: 'Etang' },
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
        },
        min: -10,
        max: 110
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
  var graphTurbineOption = {
    title: { text: 'Turbine' },
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
        text: 'Ouverture (%)'
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
      name: 'Ouverture (%)',
      data: [],
      color: '#05918a',
      
      },
      { 
        data:[],
        name: 'Cible (%)',
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
  //chartNiveau.series[0].addPoint([(new Date()).getTime(), 10],true ,false,true);
  chartTurbine = Highcharts.stockChart('chartTurbine',graphTurbineOption)
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
  // websocket.addEventListener("message", (event) => {
  //   console.log("dacc" , event);
  // })
  // websocket.addEventListener("message", (event) => {
  //   console.log("dacc2" , event);
  // })
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

  var dt = new Date()
  for (let i = 0; i < keys.length; i++) {
    const element = keys[i];
    //console.log(element);
    var el  = document.getElementById(element);
    if(typeof(el) != 'undefined' && el != null){
      el.innerHTML = data[element];
    }else{
      //console.log("element: " + element + " not exist");
    }
    if (element == "Mode") {
      desactivateModes();
      var el = modes.querySelector(`[data-num="${data[element]}"]`)

      activateMode(el);
    }
    if (element == "ratioNiveauEtang") {
      chartNiveau.series[0].addPoint([dt.getTime(), data[element]],true ,false,true);
    }
    if (element == "targetNiveauEtang") {
      chartNiveau.series[2].addPoint([dt.getTime(), data[element]],true ,false,true);
    }
    if (element == "positionVanne") {
      chartTurbine.series[0].addPoint([dt.getTime(), data[element]],true ,false,true);
    }
    if (element == "RangePosVanneTarget") {
      chartTurbine.series[1].addPoint([dt.getTime(), data[element]],true ,false,true);
      el.value = data[element]
    }
  }
}
function onLoad(event) {
  initWebSocket();
  

  var rangePosVanne = document.querySelector("#RangePosVanneTarget")


  rangePosVanne.addEventListener('change',(e) => {
    console.log(e);
    websocket.send(e.target.id + " " + e.target.value)
  })

  var computedbtn = document.querySelectorAll(".computed")
  computedbtn.forEach(element => {
    element.addEventListener('click',(e) =>{
      websocket.send("Action:" + e.target.dataset.node + ":" + e.target.dataset.action +";")
    });
  
  })

param = document.querySelectorAll(".param")
  param.forEach(element =>{
    if (element.type == "text" || element.type == "number") {
      element.addEventListener('change',  el =>{
        console.log("param onchange");
        websocket.send(`kp=${el.target.value}`)
      })
      
    } else if (element.type == "button") {
      
    } else if (element.type == "range") {
      
    }
  })
  
}


