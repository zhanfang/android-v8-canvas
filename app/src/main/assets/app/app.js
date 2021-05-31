// flutter app

var vnode = {
  type: 'page',
  props: {
    appBar: {
        type: 'appBar',
        props: {
            title: 'Homepage',
            titleColor: [200, 200, 200, 1]
        }
    }
  },
  children: [{
    type: 'text',
    props: {
        text: 'hello zhanfang',
        size: 20,
        align: 'center'
    }
  }]
};

function pushPage(params) {
    return {
        method: 'pushPage',
        params
    };
}

//call_to_java(b);
call_to_flutter(JSON.stringify(pushPage(vnode)));