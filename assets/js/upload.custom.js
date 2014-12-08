
        jQuery(document).ready(function()
        {
            $( "#sortable" ).sortable();
            $( "#sortable" ).disableSelection();
            $("a[rel^='justCar']").prettyPhoto({social_tools:false});

            var settings = {
                // url: "http://dev.clippingpathfamily.com/upload/start_upload",
                url: "http://justcartrade.com/ka_upload/upload/",
                formData: {},
                dragDrop: true,
                showProgress: true,
                showStatusAfterSuccess: false,
                fileName: "myfile",
                method: "POST",
                maxFileSize:1024*1024*300,
                allowedTypes:"psd,jpg,png,gif,doc,pdf,zip,ai,eps,raw,tiff,nef,jpeg,bmp,dng,rar,tar,gz,iso,bz2,lz,rz,tar.gz,tgz,tar.bz2,zipx,zz,7z",  
                returnType:"json",
                onSelect:function(files){ 
                    console.log('files: '+settings);
                    return true;
                },            
                onSubmit: function (files, xhr) {
                    console.log('files: '+files);
                    console.log('xhr: '+xhr);
                },
                onError: function (files, status, message) {
                   console.log('files: '+files);
                   console.log('status: '+status);
                   console.log('message: '+message);
                },
                onSuccess:function(files,data,xhr)
                {
                   console.log('files: '+files);
                   console.log('xhr: '+xhr);
                   console.log('data: '+data);
                },
                showDelete:false,
                deleteCallback: function(data,pd)
                {
                    for(var i=0;i<data.length;i++)
                    {
                        jQuery.post("delete.php",{op:"delete",name:data[i]},
                        function(resp, textStatus, jqXHR)
                        {
                            //Show Message  
                            jQuery("#status").append("<div>File Deleted</div>");      
                        });
                    }      
                    pd.statusbar.hide(); //You choice to hide/not.
                }
            }
            // console.log(settings);
            var uploadObj = jQuery("#mulitplefileuploader").uploadFile(settings);
        });