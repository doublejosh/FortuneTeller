import React from 'react'
import Fortune from './Fortune'
import { css } from 'glamor'

import heart from '../static/img/heart.svg'
import diamond from '../static/img/diamond.svg'
import skull from '../static/img/skull.svg'
import eye from '../static/img/eye.svg'
import wave from '../static/img/wave.svg'

let container = css({
	'& h3': {
		fontSize: '1.75rem',
		fontWeight: 'normal',
		color: '#000',
		padding: '0 0 1rem 3rem',
		margin: '0',
		backgroundRepeat: 'no-repeat',
		backgroundPosition: 'left bottom',
		backgroundSize: 'contain',
		'@media (min-width: 600px)': {
			paddingLeft: '3rem',
		},
	},

	'&.heart h3': {
		backgroundImage: `url(${heart})`,
	},
	'&.fortune h3': {
		backgroundImage: `url(${diamond})`,
	},
	'&.health h3': {
		backgroundImage: `url(${skull})`,
	},
	'&.self h3': {
		backgroundImage: `url(${eye})`,
	},
	'&.wave h3': {
		backgroundImage: `url(${wave})`,
	},

	'& ul': {
		paddingLeft: '0',
		margin: '0',

		'& li': {
			padding: '0.5rem 0 0.5rem 0.5rem',
			marginBottom: '0.5rem',
			boxShadow: '-3px 3px 0px rgba(0, 0, 0, 0.4)',
			background: 'rgba(255, 255, 255, 0.1)',
			position: 'relative',
			listStyleType: 'none',

			'@media (min-width: 600px)': {
				padding: '0.75rem 0 0.75rem 0.75rem',
				marginBottom: '1rem',
			},

			'& p': {
				position: 'relative',
				zIndex: '10',
				margin: '0',
				fontSize: '1.25rem',
				paddingRight: '4rem',
				'@media (min-width: 600px)': {
					paddingRight: '235px',
				},
			},
			'& .data': {
				marginTop: '0.5rem',
				fontSize: '0.75rem',
				fontWeight: '100',
				color: '#000',
			},

			'& .score-badge': {
				position: 'absolute',
				right: '0',
				top: '0',
				height: '100%',
				background: `linear-gradient(90deg,
					rgba(36, 58, 158, 0.5) 0%,
					rgb(36, 58, 158) 100%
				)`,
				textAlign: 'right',
				display: 'flex',
				alignItems: 'center',
				justifyContent: 'flex-end',

				'& .number': {
					color: '#000',
					fontSize: '1.5rem',
					padding: '0 1rem',
				},
			},
			'& .score-badge.low-score': {
				color: 'rgba(0, 0, 0, 0.3)',
				background: 'none',
			},
		},
	},
})

let countBadge = css({
	textAlign: 'right',
	marginRight: '0.5rem',
	fontSize: '1.5rem',
	color: 'rgba(0, 0, 0, 0.6)',
})

export default props => {
	const getScore = (votes, shown) => {
		if (shown === 0 || votes > shown) return 0
		return Math.round((parseInt(votes) / parseInt(shown)) * 100)
	}

	let totalShown = 0,
		totalVotes = 0

	console.log(props.css)

	return (
		<section {...container} {...props.style} className={props.id}>
			<h3>{props.label}</h3>
			<ul className="fortunes">
				{props.fortunes &&
					props.fortunes
						.filter(f => f.category === props.id)
						.map(f => ({ ...f, score: getScore(f.votes, f.shown) }))
						.sort((a, b) => b.score - a.score)
						.map((f, i) => {
							totalShown += f.shown
							totalVotes += f.votes
							return <Fortune key={i} {...f} />
						})}
			</ul>
			<div {...countBadge}>
				{totalVotes}/{totalShown}
				{' (' + Math.round((totalVotes / totalShown) * 100)}%)
			</div>
		</section>
	)
}
