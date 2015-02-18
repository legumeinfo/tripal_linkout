<?php

/**
 * Menu callbacks.
 */

function tripal_linkout_json($genus,$species,$gene,$transcript) {
    $link =  [];

    if ($genus == 'Glycine' && $species == 'max') {
        $link['href'] =  'http://www.soybase.org/sbt/search/search_results.php?category=FeatureName&search_term=' . $gene;
        //$link['href'] =  'http://www.soybase.org/gb2/gbrowse/gmax2.0/?q=' . $gene . ';dbid=gene_models_wm82_a2_v1';
        $link['text'] = 'view at SoyBase: ' . $gene;
        
    }
    else if ($genus == 'Phaseolus' && $species == 'vulgaris') {
        $link['href'] =  'http://phytozome.jgi.doe.gov/pz/portal.html#!$gene?organism=Pvulgaris&searchText=locusName:' . $gene;
        $link['text'] = 'view at Phytozome: ' . $gene;
        
    }
    else if ($genus == 'Medicago' && $species == 'truncatula') {
        $link['href'] =  'http://medicago.jcvi.org/medicago/jbrowse/?data=data%2Fjson%2Fmedicago&loc='.$transcript.'&tracks=DNA%2Cgene_models&highlight=';
        $link['text'] = 'view at JCVI: ' . $transcript;
        
    }
    else if ($genus == 'Arachis' && $species == 'duranensis') {
        //$link['href'] =  'http://peanutbase.org/gb2/gbrowse/Aradu1.0/?q=' . $gene;
        //$link['href'] =  'http://peanutbase-stage.agron.iastate.edu/gb2/gbrowse/Aradu1.0/?q=' . $gene;
        $link['href'] =  'http://peanutbase.org/gb2/gbrowse/Aradu1.0/?q=' . $gene . ';dbid=gene_models';
        //$link['href'] =  'http://peanutbase-stage.agron.iastate.edu/gb2/gbrowse/Aradu1.0/?q=' . $gene . ';dbid=gene_models';
        $link['text'] = 'view at PeanutBase: ' . $gene;
        
    }
    else if ($genus == 'Arachis' && $species == 'ipaensis') {
        //$link['href'] =  'http://peanutbase.org/gb2/gbrowse/Araip1.0/?q=' . $gene;
        //$link['href'] =  'http://peanutbase-stage.agron.iastate.edu/gb2/gbrowse/Araip1.0/?q=' . $gene;
        $link['href'] =  'http://peanutbase.org/gb2/gbrowse/Araip1.0/?q=' . $gene . ';dbid=gene_models';
        //$link['href'] =  'http://peanutbase-stage.agron.iastate.edu/gb2/gbrowse/Araip1.0/?q=' . $gene . ';dbid=gene_models';
        $link['text'] = 'view at PeanutBase: ' . $gene;
        
    }
    else if ($genus == 'Arabidopsis' && $species == 'thaliana') {
        $link['href'] =  'http://www.araport.org/locus/' . $gene;
        $link['text'] = 'view at Arabidopsis Information Portal: ' . $gene;
        
    }

    drupal_json_output($link);  
    drupal_exit();
}


?>