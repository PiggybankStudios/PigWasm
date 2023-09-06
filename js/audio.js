
console.log("Audio worker has started");

self.onmessage = function(message)
{
	console.log("Audio worker got message", message);
}

// function audioUpdate()
// {
// 	console.log("Audio update...");
// 	setTimeout(audioUpdate, 1000);
// }
// setTimeout(audioUpdate, 1000);

// var testVar = 0;
// while (testVar < 2000000000)
// {
// 	testVar++;
// }
// console.log("Audio thread is done!");