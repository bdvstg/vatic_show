# vatic_show
this program is wrote for the one who always complain can't control VATIC happily...

cause VATIC will do interpolation

she complain bounding box can not follow her way every frame

so..., vatic_show has created


## function:
After use VATIC, dump result by pascal format, you can use this program to adjust the result.
###### vatic_show can
  1. modify bounding box
        1. change region position/size
        2. delete/add bounding box
  2. change class name
  3. move frame into recycle
  4. browser result

###### problem:
  - it have chinese char



###### Tip:
output VATIC result by pascal format:

  turkic dump VIDEO_ID --output OUTPUT_FOLDER --pascal --pascal-skip 1

--pascal-skip let it output every frame, else it may output a frame per 15 frame in default

if JPEGImage folder is empty, try use sudo



###### about this project:
this project use cmake, my default IDE is vs2013, use opencv 2.4.9, qt5
  
