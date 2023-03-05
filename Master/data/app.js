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

let graphDataLoaded = false;

let mode1 = new modes_C("mode0",0);
let mode2 = new modeBasic("mode1",1);
let mode3 = new modes_C("mode2",2);
let activeMode = 0;
var modes_li
var modes
var param
var modesArray = [mode1,mode2,mode3];
var sliderTimingBudget

let modalDisconnected;
let chartNiveau 
let chartTurbine 
function initTerminal(){
  console.log("init terminal");
  const form = document.querySelector("#command-form")
  const input = document.querySelector("#command-input")

  form.addEventListener("submit", (e) => {
    e.preventDefault();
    const command = input.value;
    terminalAdd("command",command)
  }
)}
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

function terminalAdd(type,text) {
    const terminal = document.getElementById("terminal");
    const n = terminal.lastChild
    if (n != undefined || n != null) {
      
      n.removeChild(n.lastChild)
    }
    const p = document.createElement("p")
    const dt = new Date();
    p.classList.add(type)
    p.appendChild(document.createTextNode("> [" + dt.toLocaleTimeString() +"] " + text))
    
    const cursor = document.createElement("span");
    cursor.textContent ="█"
    cursor.classList.add("cursor");
    cursor.classList.add(type);
    p.appendChild(cursor);
    terminal.appendChild(p)
    terminal.scrollTop = terminal.scrollHeight;
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

  modalDisconnected = new bootstrap.Modal(document.getElementById('modalDisconnected'))
  
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
            count: 1,
            type: 'day',
            text: '1D'
        }, {
            count: 1,
            type: 'week',
            text: '1W'
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
    series: [ 
      {
        data: [],
        name: 'Niveau (%)',
        color: '#007acc',
      }
      // { 
      //   data:[],
      //   name: 'Ouverture (%)',
      //   color: '#FFAA8a' 
      // },
      // { 
      //   data:[],
      //   name: 'Cible (%)',
      //   color: '#FFC78A' 
      // }
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
    yAxis: [{
        title: {
            text: '(%)'
        }
    },{
      title:{ text:"tacky"}
    }],
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
        name: 'tacky (rpm)',
        color: '#ee931f',
        yAxis: 1
      }
      //, {
    //       name: 'John',
    //       data: [5, 7, 3]
    //   }
    ]
  };

  Highcharts.ajax({
    url: 'data.csv',
    dataType: 'text',
    success: function (data) {
      var lines = data.split('\n');
      lines.forEach(function(line,lineNo) {
        var items = line.split(',');
        // header line containes categories  
        if (lineNo == 0) {  
          items.forEach(function(item, itemNo) {  
              //if (itemNo > 0) options.xAxis.categories.push(item);  
          });  

        } 
        // the rest of the lines contain data with their name in the first position  
        else {
          var seriesNiveau = {
            data:[]
          }
          var seriesTurbine = {
            data: []
          }
          var dt = new Date(((items[0]-3600)) * 1000).getTime();
          //console.log("ajax: " , dt);
          graphNiveauOption.series[0].data.push([dt,parseFloat(items[2])]);
          graphTurbineOption.series[0].data.push([dt,parseFloat(items[4])]);
          graphTurbineOption.series[1].data.push([dt,parseFloat(items[3])]);
          graphTurbineOption.series[2].data.push([dt,parseFloat(items[1])]);

        }
      })
      chartNiveau = Highcharts.stockChart('chartNiveau',graphNiveauOption)
      chartTurbine = Highcharts.stockChart('chartTurbine',graphTurbineOption)
    }
  })
  //console.log("fini" , graphNiveauOption.series[0].data);
  
  initWebSocket(); 

 
  initTerminal()

   sliderTimingBudget = new rSlider({
    target: '#sliderTimingBudget',
    values: [15, 20, 33, 50, 100, 200, 500],
    range: false,
    set:[100]

  })
  
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
  modalDisconnected.hide();
}
function onClose(event) {
  console.log('Connection closed');
  //alert('Connection closed');
  modalDisconnected.show();
  setTimeout(initWebSocket, 2000);
  Alpine.store("Ws.etablished",false);
}
function wsData(d) {
  var keys = Object.keys(d)
  try {
    

    var dt =  Date.now();
    for (let i = 0; i < keys.length; i++) {
      const element = keys[i];

      var el  = document.getElementById(element);
      if(typeof(el) != 'undefined' && el != null){
        if (el.type == "checkbox") {
          el.checked = d[element]
        } else {
          el.innerHTML = d[element];
        }
      }else{
        console.log("element: " + element + " not exist");
      }
      if (element == "Mode") {
        desactivateModes();
        var el = modes.querySelector(`[data-num="${d[element]}"]`)

        activateMode(el);
      }
      if (element == "ratioNiveauEtang") {
        chartNiveau.series[0].addPoint([dt, d[element]],true ,false,true);
      }
      if (element == "targetNiveauEtang") {
        chartNiveau.series[2].addPoint([dt, d[element]],true ,false,true);
      }
      if (element == "positionVanne") {
        chartTurbine.series[0].addPoint([dt, d[element]],true ,false,true);
      }
      if (element == "RangePosVanneTarget") {
        chartTurbine.series[1].addPoint([dt, d[element]],true ,false,true);
        el.value = d[element]
      }
      if (element == "tacky") {
        chartTurbine.series[2].addPoint([dt, d[element]],true ,false,true);
        el.value = d[element]
      }
      
    }
  } catch (error) {
   console.error(error); 
  }
}
function onMessage(event) {

  var data = JSON.parse(event.data);
  
  if (data.data) {
    wsData(data.data)
  }
  if (data.monitor) {
    console.log(data.monitor);
    terminalAdd("info",data.monitor)
  }
}
function sendAction(node,action) {
  console.log("sendAction: " , node , action);
  terminalAdd("command","sendAction: " + node + action);
  websocket.send("Action:" + node + ":" + action + ";")
}
function onLoad(event) {
  //initWebSocket();
  

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

  var computedRange = document.querySelectorAll(".computed-range")
  computedRange.forEach(element => {
    element.addEventListener('change', (e) => {
      console.log(e);
      var node = e.target.dataset.node
      var cmd = e.target.dataset.action
      var val = e.target.value
      console.log(node,cmd,val);
      sendAction(node,cmd + "=" + val)

    })
  })

param = document.querySelectorAll(".param")
  param.forEach(element =>{
    if (element.type == "text" || element.type == "number") {
      element.addEventListener('change',  el =>{c
        let param = el.target.dataset.param;
        websocket.send(`${param}=${el.target.value}`)
      })
      
    } else if (element.type == "button") {
      
    } else if (element.type == "range") {
      
    }
  })
  
}


