if (Notification) {
    Notification.requestPermission().then(function(permission){
        if (permission === 'granted') {
            const not = new Notification('Titre',{
                body: 'corps',
                icon: '/icon.png'
            });
        } else {

        }
    })
}


const worker = new Worker('worker.js');
worker.onmessage = function(event){
    console.log(event.data);
}


function send() {
    worker.postMessage({message:'Hello worker !'})
}