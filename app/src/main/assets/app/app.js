// flutter app

var a = {
    method: 'updateWidget',
    params: {
        b: 2
    }
};

function pushPage() {

}

function Page() {
    
}


var b = JSON.stringify(a);
call_to_java(b);
call_to_flutter(b);