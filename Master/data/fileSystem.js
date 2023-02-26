function humanReadableSize(bytes, decimals = 2) {
    if (bytes === 0) return '0 Bytes';
    const k = 1024;
    const dm = decimals < 0 ? 0 : decimals;
    const sizes = ['Bytes', 'kB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(dm)) + ' ' + sizes[i];
}

document.addEventListener('alpine:init', () => {
    Alpine.data('filesData', () => ({
      files:null,
      sortCol:null,
      sortAsc:false,
      async init() {
        let resp = await fetch('/api/fs');
        let data = await resp.json();
        data.SPIFFS.forEach((d,i) => d.id = i);
        this.files = data.SPIFFS;
        console.log(this.files);
      },
      sort(col) {
        if(this.sortCol === col) this.sortAsc = !this.sortAsc;
        this.sortCol = col;
        this.files.sort((a, b) => {
          if(a[this.sortCol] < b[this.sortCol]) return this.sortAsc?1:-1;
          if(a[this.sortCol] > b[this.sortCol]) return this.sortAsc?-1:1;
          return 0;
        });
      },
      async send(file){
        console.log(file.id);
        console.log(file.name);
        const fData = new FormData();
        fData.append('fileName', file.path)
        let res = await fetch("/sendFile",{
            method:'post',
            body: fData
        })
        if (res.ok) {
            console.log('ok');
        }
      },
      async sendOta(file){
        const fData = new FormData();
        fData.append('fileName', file.path)
        fData.append('Ota',true);
        let res = await fetch("/sendFile",{
            method:'post',
            body: fData
        })
        if (res.ok) {
            console.log('ok');
        }
      },
      async deletet(file){
        const fData = new FormData();
        fData.append('fileName', file.path)
       
        let res = await fetch("/api/fs",{
            method:'delete',
            body: fData
        })
        if (res.ok) {
            console.log('ok');
            this.files.splice(this.files.indexOf(file), 1);
        }
      },
      test(id){
        console.log(id)
        console.log(this.files[id]);
      },
      test2(file){
        console.log(file)
        //console.log(this.files[id]);
      }
    }))
  });