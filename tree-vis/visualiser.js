
window.addEventListener('DOMContentLoaded', function(){
  var cy = window.cy = cytoscape({
    container: document.getElementById('cy'),

    boxSelectionEnabled: false,
    autounselectify: true,

    layout: {
      name: 'dagre'
    },

    style: [
      {
        selector: 'node',
        style: {
          'background-color': '#11479e'
        }
      },

      {
        selector: 'node.leaf',
        style: {
          'label': 'data(token)',
          'color': 'white',
          'text-halign': 'center',
          'text-valign': 'center',
        }
      },

      {
        selector: 'edge',
        style: {
          'label': 'data(label)',
          'text-margin-y': '-12',
          'width': 4,
          'target-arrow-shape': 'triangle',
          'line-color': '#9dbaea',
          'target-arrow-color': '#9dbaea',
          'curve-style': 'bezier'
        }
      }
    ],

    elements: elements,
  });
});
