import QtQuick 2.15
import QtTest 1.15

TestCase {
    name: "SmmMonitorArayuzTestleri"

    function getStatusColor(val) {
        if(val === 0) return "64748b";
        if(val >= 95) return "10b981";
        if(val >= 90) return "f59e0b";
        return "ef4444";
    }
    function test_renk_durumlari(){
        compare(getStatusColor(0), "64748b", "Sensör boşken gri dönmeli");
        compare(getStatusColor(98), "10b981", "Sağlıklı oksijen yeşil dönmeli");
        compare(getStatusColor(85), "ef4444", "kritik oksijen kırmızı dönmeli");
    }
}
