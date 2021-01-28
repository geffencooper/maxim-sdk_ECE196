sudo ./openocd -f interface/cmsis-dap.cfg -f target/max78000.cfg -s ./scripts -c "program ../../Examples/MAX78000/CNN/Geffen_mnist/build/max78000.elf verify reset exit"
#sudo ./openocd -f interface/cmsis-dap.cfg -f target/max78000.cfg -s ./scripts -c "program ../../Examples/MAX78000/CNN/mnist/build/max78000.elf verify reset exit"
