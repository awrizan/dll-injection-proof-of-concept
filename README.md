# dll-injection-proof-of-concept
this is sort of just me proving that i can inject a dll, but this is in no way practical. whenever you inject, the program closes because i didnt add something that hooks api calls. i could impliment that but i dont feel like it

also i forgot to mention that when you make a hinstance to the dll, you need to have a function that only runs the dll if it is in the correct process. you gotta do this bc if you dont the dll will run when the hinstance is created, instead of when it is attached to the target process. you can just recycle the findProcId function. the function should be inside of the dll and would just be a if statement that wraps around the switch with like the DLL_PROCESS_ATTACH (or whatever its called i forget exactly what it is). ill probably upload a dll example at one point but i dont feel like it rn
