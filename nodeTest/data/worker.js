onmessage = function (event) {
    this.postMessage({message: 'Hello main thread !'})
}