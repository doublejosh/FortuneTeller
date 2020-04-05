import React from 'react'
import Fortune from './Fortune'
import { css } from 'glamor'

import heart from '../static/img/heart.svg'
import diamond from '../static/img/diamond.svg'
import skull from '../static/img/skull.svg'
import eye from '../static/img/eye.svg'
import wave from '../static/img/wave.svg'

export default props => {
	const getScore = (votes, shown) => {
		if (shown === 0 || votes > shown) return 0
		return Math.round((parseInt(votes) / parseInt(shown)) * 100)
	}

	let totalShown = 0,
		totalVotes = 0

	console.log(props.css)

	return (
		<section
			{...css({
				'&.heart h3': { backgroundImage: `url(${heart})` },
				'&.fortune h3': { backgroundImage: `url(${diamond})` },
				'&.health h3': { backgroundImage: `url(${skull})` },
				'&.self h3': { backgroundImage: `url(${eye})` },
				'&.wave h3': { backgroundImage: `url(${wave})` },
			})}
			{...props.style}
			className={props.id}>
			<h3
				{...css({
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
				})}>
				{props.label}
			</h3>
			<ul {...css({ paddingLeft: '0', margin: '0' })}>
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
			<div
				{...css({
					textAlign: 'right',
					marginRight: '0.5rem',
					fontSize: '1.5rem',
					color: 'rgba(0, 0, 0, 0.6)',
				})}>
				{totalVotes}/{totalShown}
				{' (' + Math.round((totalVotes / totalShown) * 100)}%)
			</div>
		</section>
	)
}
