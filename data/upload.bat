@echo off 
curl %1/format
ping 1.1.1.1 -n 1 -w 1000 > nul
curl -F "file=@jquery.mobile-1.4.5.min.css.gz" %1/edit
@echo.
@echo jquery.mobile-1.4.5.min.css.gz
ping 1.1.1.1 -n 1 -w 1000 > nul
curl -F "file=@jquery-1.11.1.min.js.gz" %1/edit
@echo jquery-1.11.1.min.js.gz
ping 1.1.1.1 -n 1 -w 1000 > nul
curl -F "file=@jquery.mobile-1.4.5.min.js.gz" %1/edit
@echo jquery.mobile-1.4.5.min.js.gz
rem ping 1.1.1.1 -n 1 -w 1000 > nul
rem curl -F "file=@jquery.mobile-1.4.5.min.map.gz" %1/edit
rem @echo jquery.mobile-1.4.5.min.map.gz
ping 1.1.1.1 -n 1 -w 1000 > nul
curl -F "file=@edit.htm.gz" %1/edit
@echo edit
rem ping 1.1.1.1 -n 1 -w 1000 > nul
rem curl -F "file=@favicon.ico" %1/edit
rem @echo favico
ping 1.1.1.1 -n 1 -w 1000 > nul
curl -F "file=@graphs.js.gz" %1/edit
@echo graphs
ping 1.1.1.1 -n 1 -w 1000 > nul
curl -F "file=@ajax-loader.gif" %1/edit
@echo ajax-loader
ping 1.1.1.1 -n 1 -w 1000 > nul
curl -F "file=@index.htm" %1/edit
@echo index
ping 1.1.1.1 -n 1 -w 1000 > nul
curl -F "file=@redirect.htm" %1/edit
@echo redirect
ping 1.1.1.1 -n 1 -w 1000 > nul
curl -F "file=@gpio.txt" %1/edit
@echo gpio
ping 1.1.1.1 -n 1 -w 1000 > nul
curl -F "file=@ssdp.txt" %1/edit
@echo ssdp
ping 1.1.1.1 -n 1 -w 1000 > nul
curl -F "file=@mqtt.txt" %1/edit
@echo mqtt
ping 1.1.1.1 -n 1 -w 1000 > nul
curl %1/reset
pause