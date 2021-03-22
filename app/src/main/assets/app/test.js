var a = 1;
var b = 3;

var c = {
    a,
    b,
    c: {
        d: [3, 4, 5]
    }
};

console.log(c);


var Context2D = bindings.Context2D;
var ctx = new Context2D('2d');

ctx.fillStyle = 'green';
ctx.fillRect(10, 10, 150, 100);

ctx.clearRect(10, 10, 100, 100);

console.log(ctx.fillStyle);
console.log(ctx.__proto__.constructor.name);
