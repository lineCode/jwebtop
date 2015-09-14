﻿using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

namespace JWebTop_CSharp_Demo {
    class DemoBrowserCtrl:JWebTop.JWebtopJSONDispater {
        public string dispatcher(long browserHWnd, string json) {
            Debug.WriteLine("分发浏览器JS，浏览器句柄=" + browserHWnd + "，" + json);
            JObject jo=JObject.Parse(json);
            return "{\"a\":\"a是字符串\",\"b\":123}";
        }

        public void resetThreadClassLoader() {
            //
        }
    }
}
